using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using LiteScript.Interop;

namespace LiteScript
{
    /// <summary>
    /// Namespaces resolution of a script context (LiteScript::Namer)
    /// </summary>
    public sealed class Namer
    {
        /// <summary>
        /// Constructor of an empty namer
        /// </summary>
        public Namer(Memory memory)
        {
            this.Memory = memory ?? throw new ArgumentNullException(nameof(memory));
        }

        /// <summary>
        /// Constructor of a namer that uses the global namespace
        /// </summary>
        public Namer(Variable globalNamespace) : this(globalNamespace.Memory)
        {
            this.Global = globalNamespace;
            this.Current = globalNamespace;
        }

        /// <summary>
        /// The memory of the namespaces
        /// </summary>
        public Memory Memory { get; }

        /// <summary>
        /// The global namespace
        /// </summary>
        public Variable? Global { get; set; }

        /// <summary>
        /// The namespace selected by the "namespace" instruction
        /// </summary>
        public Variable? Current { get; set; }

        /// <summary>
        /// The pushed scopes (namespaces of local variables or classes)
        /// </summary>
        public List<Variable?> Scopes { get; } = new List<Variable?>();

        private byte[] Serialize(out BufferWriter writer)
        {
            writer = new BufferWriter();
            writer.Namer(this);
            return writer.ToArray();
        }

        /// <summary>
        /// Find a variable in scopes, then in the current namespace and then in the global namespace
        /// </summary>
        /// <returns>The variable or null if not found</returns>
        public Variable? Find(string name)
        {
            byte[] buffer = this.Serialize(out BufferWriter writer);
            this.Memory.Check(NativeMethods.ls_namer_find(buffer, (UIntPtr)buffer.Length, Utf8.Encode(name), out IntPtr v));
            writer.KeepAlive();
            return this.Memory.WrapOptional(v);
        }

        /// <summary>
        /// Declare a variable in the last namespace scope (or the current namespace)
        /// </summary>
        public void Declare(string name, Variable? value)
        {
            byte[] buffer = this.Serialize(out BufferWriter writer);
            this.Memory.Check(NativeMethods.ls_namer_declare(buffer, (UIntPtr)buffer.Length, Utf8.Encode(name), Memory.HandleOf(value)));
            writer.KeepAlive();
            GC.KeepAlive(value);
        }

        /// <summary>
        /// Push a scope (a namespace or a class)
        /// </summary>
        public void Push(Variable scope)
        {
            if (scope != null && (scope.Is(ScriptType.Namespace) || scope.Is(ScriptType.Class)))
                this.Scopes.Add(scope);
        }

        /// <summary>
        /// Pop the last scope if it's the indicated one
        /// </summary>
        public void Pop(Variable? scope)
        {
            if (this.Scopes.Count > 0 && Variable.SameObject(this.Scopes[this.Scopes.Count - 1], scope))
                this.Scopes.RemoveAt(this.Scopes.Count - 1);
        }
    }

    /// <summary>
    /// A function call seen by a debugger (LiteScript::State::FrameInfo)
    /// </summary>
    public sealed class FrameInfo
    {
        internal FrameInfo(int list, int line, Namer namer, Variable? self, IReadOnlyList<Variable?> args)
        {
            this.List = list;
            this.Line = line;
            this.Namer = namer;
            this.Self = self;
            this.Args = args;
        }

        /// <summary>
        /// The list of the position: the next instruction for the innermost frame, the instruction of
        /// the call in progress for the others
        /// </summary>
        public int List { get; }

        /// <summary>
        /// The index of the instruction of the position
        /// </summary>
        public int Line { get; }

        /// <summary>
        /// The variables visible in the frame (the last scope that is a namespace contains the local variables)
        /// </summary>
        public Namer Namer { get; }

        /// <summary>
        /// The value of "this"
        /// </summary>
        public Variable? Self { get; }

        /// <summary>
        /// The arguments
        /// </summary>
        public IReadOnlyList<Variable?> Args { get; }
    }

    /// <summary>
    /// The execution state of a script: instructions, position, stacks and namespaces (LiteScript::State).
    /// The memory must outlive the state (disposing the memory disposes the state).
    /// </summary>
    public sealed class State : IDisposable
    {
        // The native state (zero when disposed)
        private IntPtr handle;

        /// <summary>
        /// Constructor of an empty state
        /// </summary>
        /// <param name="memory">The main memory</param>
        public State(Memory memory)
        {
            this.Memory = memory ?? throw new ArgumentNullException(nameof(memory));
            this.handle = NativeMethods.ls_state_new(memory.Handle);
            if (this.handle == IntPtr.Zero)
                throw new OutOfMemoryException("can't create the state");
            Native.RegisterState(this);
            memory.AddState(this);
        }

        /// <summary>
        /// Destructor of a state that hasn't been disposed
        /// </summary>
        ~State()
        {
            this.Destroy(true);
        }

        /// <summary>
        /// Destroy the state
        /// </summary>
        public void Dispose()
        {
            this.Destroy(false);
            GC.SuppressFinalize(this);
        }

        internal void Destroy(bool finalizing)
        {
            IntPtr h = Interlocked.Exchange(ref this.handle, IntPtr.Zero);
            if (h == IntPtr.Zero)
                return;
            Native.UnregisterState(h);
            this.Memory.ReleaseState(h, finalizing);
        }

        /// <summary>
        /// Indicate if the state has been disposed
        /// </summary>
        public bool IsDisposed => this.handle == IntPtr.Zero;

        internal IntPtr Handle
        {
            get
            {
                IntPtr h = this.handle;
                if (h == IntPtr.Zero)
                    throw new ObjectDisposedException(nameof(State));
                if (this.Memory.IsDisposed)
                    throw new ObjectDisposedException(nameof(LiteScript.Memory));
                this.Memory.ReleasePending();
                return h;
            }
        }

        /// <summary>
        /// Return the main memory
        /// </summary>
        public Memory Memory { get; }

        private Variable Result(int status, IntPtr result)
        {
            GC.KeepAlive(this);
            this.Memory.Check(status);
            return this.Memory.WrapOptional(result) ?? this.Memory.CreateUndefined();
        }

        ////// INSTRUCTIONS //////

        /// <summary>
        /// Add a list of instructions, executed after the current ones
        /// </summary>
        /// <returns>The index of the list</returns>
        public int AddInstructions(IEnumerable<Instruction> instructions)
        {
            byte[] buffer = BufferWriter.FromInstructions(instructions);
            this.Memory.Check(NativeMethods.ls_state_add_instructions(this.Handle, buffer, (UIntPtr)buffer.Length, out ulong index));
            GC.KeepAlive(this);
            return checked((int)index);
        }

        /// <summary>
        /// Add an instruction at the end of the last list
        /// </summary>
        public void AddInstruction(Instruction instruction)
        {
            byte[] buffer = BufferWriter.FromInstruction(instruction);
            this.Memory.Check(NativeMethods.ls_state_add_instruction(this.Handle, buffer, (UIntPtr)buffer.Length));
            GC.KeepAlive(this);
        }

        /// <summary>
        /// Return the number of lists of instructions
        /// </summary>
        public int InstructionListCount => checked((int)NativeMethods.ls_state_list_count(this.Handle));

        /// <summary>
        /// Return a copy of a list of instructions
        /// </summary>
        public List<Instruction> GetInstructions(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            this.Memory.Check(NativeMethods.ls_state_get_instructions(this.Handle, (ulong)index, out IntPtr buffer, out UIntPtr size));
            GC.KeepAlive(this);
            return new BufferReader(buffer, size).Instructions();
        }

        /// <summary>
        /// Return the index of the current list of instructions
        /// </summary>
        public int ListIndex => checked((int)NativeMethods.ls_state_list_index(this.Handle));

        /// <summary>
        /// Return the index of the next instruction in the current list
        /// </summary>
        public int Line => checked((int)NativeMethods.ls_state_line(this.Handle));

        ////// EXECUTION //////

        /// <summary>
        /// Execute every remaining instruction
        /// </summary>
        /// <returns>The value on the top of the stack (undefined if the stack is empty)</returns>
        /// <exception cref="ScriptException">An error isn't caught by the script</exception>
        public Variable Execute()
        {
            int status = NativeMethods.ls_state_execute(this.Handle, out IntPtr result);
            return this.Result(status, result);
        }

        /// <summary>
        /// Execute the next instruction
        /// </summary>
        /// <returns>The value on the top of the stack (undefined if the stack is empty)</returns>
        /// <exception cref="ScriptException">An error isn't caught by the script</exception>
        public Variable ExecuteSingle()
        {
            int status = NativeMethods.ls_state_execute_single(this.Handle, out IntPtr result);
            return this.Result(status, result);
        }

        /// <summary>
        /// Execute an instruction that isn't stored in the lists (jumps are relative to the current list)
        /// </summary>
        /// <returns>The value on the top of the stack (undefined if the stack is empty)</returns>
        public Variable ExecuteInstruction(Instruction instruction)
        {
            byte[] buffer = BufferWriter.FromInstruction(instruction);
            int status = NativeMethods.ls_state_execute_instruction(this.Handle, buffer, (UIntPtr)buffer.Length, out IntPtr result);
            return this.Result(status, result);
        }

        /// <summary>
        /// Indicate if there is no instruction to execute
        /// </summary>
        public bool Finished => NativeMethods.ls_state_finished(this.Handle) != 0;

        /// <summary>
        /// Call a function (a callback or an instance with an overloaded call operator)
        /// and execute it until it returns
        /// </summary>
        /// <param name="callee">The function</param>
        /// <param name="args">The arguments</param>
        /// <returns>The returned value</returns>
        /// <exception cref="ScriptException">An error isn't caught by the function</exception>
        public Variable Call(Variable callee, params Variable?[] args)
        {
            args ??= Array.Empty<Variable?>();
            IntPtr[] handles = new IntPtr[args.Length];
            for (int i = 0; i < args.Length; i++)
                handles[i] = Memory.HandleOf(args[i]);
            int status = NativeMethods.ls_state_call(this.Handle, Memory.HandleOf(callee), handles, (UIntPtr)handles.Length, out IntPtr result);
            GC.KeepAlive(callee);
            GC.KeepAlive(args);
            return this.Result(status, result);
        }

        /// <summary>
        /// Call a function with a list of arguments
        /// </summary>
        public Variable Call(Variable callee, IEnumerable<Variable?> args)
        {
            return this.Call(callee, new List<Variable?>(args).ToArray());
        }

        /// <summary>
        /// Change the maximum number of nested calls (deeper calls raise an error)
        /// </summary>
        public void SetMaxCallDepth(long depth)
        {
            if (depth < 0)
                throw new ArgumentOutOfRangeException(nameof(depth));
            NativeMethods.ls_state_set_max_call_depth(this.Handle, (ulong)depth);
        }

        ////// VARIABLES //////

        /// <summary>
        /// Find a variable visible from the current position
        /// </summary>
        /// <returns>The variable or null if not found</returns>
        public Variable? GetVariable(string name)
        {
            this.Memory.Check(NativeMethods.ls_state_get_variable(this.Handle, Utf8.Encode(name), out IntPtr v));
            GC.KeepAlive(this);
            return this.Memory.WrapOptional(v);
        }

        /// <summary>
        /// Declare a variable in the global namespace
        /// </summary>
        public void DeclareVariable(string name, Variable? value)
        {
            this.Memory.Check(NativeMethods.ls_state_declare_variable(this.Handle, Utf8.Encode(name), Memory.HandleOf(value)));
            GC.KeepAlive(this);
            GC.KeepAlive(value);
        }

        /// <summary>
        /// Return the global namespace
        /// </summary>
        public Variable GlobalNamespace
        {
            get
            {
                IntPtr v = NativeMethods.ls_state_global_namespace(this.Handle);
                GC.KeepAlive(this);
                return this.Memory.Wrap(v);
            }
        }

        /// <summary>
        /// Select the current namespace ("global" or names separated by dots, created if needed)
        /// </summary>
        public void UseNamespace(string path = "global")
        {
            this.Memory.Check(NativeMethods.ls_state_use_namespace(this.Handle, Utf8.Encode(path)));
            GC.KeepAlive(this);
        }

        /// <summary>
        /// Return the value of "this" in the current function (undefined if not set)
        /// </summary>
        public Variable GetThis()
        {
            int status = NativeMethods.ls_state_get_this(this.Handle, out IntPtr v);
            return this.Result(status, v);
        }

        /// <summary>
        /// Return the number of arguments of the current function
        /// </summary>
        public int ArgsCount => checked((int)NativeMethods.ls_state_args_count(this.Handle));

        /// <summary>
        /// Return an argument of the current function (undefined if it doesn't exist)
        /// </summary>
        public Variable GetArg(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            int status = NativeMethods.ls_state_get_arg(this.Handle, (ulong)index, out IntPtr v);
            return this.Result(status, v);
        }

        /// <summary>
        /// Return the stack of operations (from the bottom to the top)
        /// </summary>
        public List<Variable?> GetStack()
        {
            this.Memory.Check(NativeMethods.ls_state_get_stack(this.Handle, out IntPtr buffer, out UIntPtr size));
            GC.KeepAlive(this);
            return new BufferReader(buffer, size, this.Memory).Variables();
        }

        /// <summary>
        /// Return the number of nested frames (calls and scopes)
        /// </summary>
        public int Depth => checked((int)NativeMethods.ls_state_depth(this.Handle));

        ////// DEBUGGING //////

        /// <summary>
        /// Return the calls in progress, from the global scope to the current function
        /// (the scopes of "push.nsp" are merged in their function, the native functions are omitted)
        /// </summary>
        public List<FrameInfo> GetFrames()
        {
            this.Memory.Check(NativeMethods.ls_state_get_frames(this.Handle, out IntPtr buffer, out UIntPtr size));
            GC.KeepAlive(this);
            var reader = new BufferReader(buffer, size, this.Memory);
            int count = checked((int)reader.U32());
            var frames = new List<FrameInfo>(count);
            for (int i = 0; i < count; i++)
            {
                int list = checked((int)reader.U64());
                int line = checked((int)reader.U64());
                Namer namer = reader.Namer();
                Variable? self = reader.Var();
                List<Variable?> args = reader.Variables();
                frames.Add(new FrameInfo(list, line, namer, self, args));
            }
            return frames;
        }

        /// <summary>
        /// Define a function called before each instruction of the lists (null to remove it).
        /// The function can read the state and call Evaluate, it's not called during an evaluation.
        /// Its exceptions are thrown by the method that executes the state.
        /// </summary>
        public void SetDebugHook(Action<State>? hook)
        {
            IntPtr target = hook != null ? Native.Alloc(hook) : IntPtr.Zero;
            this.Memory.Check(NativeMethods.ls_state_set_debug_hook(this.Handle, target));
            GC.KeepAlive(this);
        }

        /// <summary>
        /// Define a function called when an error isn't caught by the script, before the stacks are
        /// cleared (null to remove it). The function can read the state and call Evaluate.
        /// </summary>
        public void SetErrorHook(Action<State, ScriptException>? hook)
        {
            IntPtr target = hook != null ? Native.Alloc(hook) : IntPtr.Zero;
            this.Memory.Check(NativeMethods.ls_state_set_error_hook(this.Handle, target));
            GC.KeepAlive(this);
        }

        /// <summary>
        /// Execute instructions in the context of the current position (the variables of the current
        /// function are visible) and restore the position and the stacks. The "try" blocks of the
        /// script don't catch the errors of the evaluation and the hooks aren't called.
        /// </summary>
        /// <param name="instructions">The instructions, the jumps are relative to their list (for example
        /// the instructions of a compiled expression without its final "value.pop")</param>
        /// <param name="maxSteps">The maximum number of instructions to execute</param>
        /// <returns>The value on the top of the stack at the end (undefined if there is none)</returns>
        /// <exception cref="ScriptException">The evaluation raises an error</exception>
        /// <exception cref="InvalidOperationException">The evaluation needs more than maxSteps instructions</exception>
        public Variable Evaluate(IEnumerable<Instruction> instructions, long maxSteps = 1000000)
        {
            if (maxSteps < 0)
                throw new ArgumentOutOfRangeException(nameof(maxSteps));
            byte[] buffer = BufferWriter.FromInstructions(instructions);
            int status = NativeMethods.ls_state_evaluate(this.Handle, buffer, (UIntPtr)buffer.Length, (ulong)maxSteps, out IntPtr result);
            return this.Result(status, result);
        }

        ////// PERSISTENCE //////

        /// <summary>
        /// Save the state (instructions, position, stacks and every object reachable from them)
        /// </summary>
        /// <exception cref="SerializationException">An object can't be saved (unregistered native function)</exception>
        public byte[] Save()
        {
            this.Memory.Check(NativeMethods.ls_state_save(this.Handle, out IntPtr buffer, out UIntPtr size));
            GC.KeepAlive(this);
            return BufferReader.Take(buffer, size);
        }

        /// <summary>
        /// Save the state in a stream
        /// </summary>
        /// <exception cref="SerializationException">An object can't be saved (unregistered native function)</exception>
        public void Save(Stream stream)
        {
            if (stream == null)
                throw new ArgumentNullException(nameof(stream));
            byte[] data = this.Save();
            stream.Write(data, 0, data.Length);
        }

        /// <summary>
        /// Replace the content of the state by a saved state.
        /// The native functions must be registered in the memory before.
        /// </summary>
        /// <exception cref="SerializationException">The data is invalid</exception>
        public void Load(byte[] data)
        {
            if (data == null)
                throw new ArgumentNullException(nameof(data));
            this.Memory.Check(NativeMethods.ls_state_load(this.Handle, data, (UIntPtr)data.Length));
            GC.KeepAlive(this);
        }

        /// <summary>
        /// Replace the content of the state by a state saved in a stream (read until its end).
        /// The native functions must be registered in the memory before.
        /// </summary>
        /// <exception cref="SerializationException">The stream is invalid</exception>
        public void Load(Stream stream)
        {
            if (stream == null)
                throw new ArgumentNullException(nameof(stream));
            using (var data = new MemoryStream())
            {
                stream.CopyTo(data);
                this.Load(data.ToArray());
            }
        }
    }
}
