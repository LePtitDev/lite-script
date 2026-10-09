using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using LiteScript.Interop;

namespace LiteScript
{
    /// <summary>
    /// A function of the application called by the scripts (LiteScript::NativeFunction).
    /// The arguments are only valid during the call (use <see cref="Variable.Duplicate"/> to keep one).
    /// A <see cref="ScriptException"/> raises an error with its value, the other exceptions become
    /// script errors with their message (the script can catch them with "try ... catch").
    /// </summary>
    /// <returns>The result (null is equivalent to undefined)</returns>
    public delegate Variable? NativeFunction(State state, IReadOnlyList<Variable?> args);

    /// <summary>
    /// The memory that owns the objects (LiteScript::Memory).
    /// Objects are destroyed as soon as they are not referenced anymore, objects that reference each
    /// other (cycles) are destroyed by the garbage collector. The garbage collector never destroys an
    /// object referenced by a <see cref="Variable"/> of the application, so it can be executed at any time.
    /// A memory and its states must be used from one thread at a time. Disposing the memory disposes its states.
    /// </summary>
    public sealed class Memory : IDisposable
    {
        // The native memory (zero when disposed)
        private IntPtr handle;

        // Variables and states released by the .NET finalizers, destroyed by the thread that uses the memory
        private readonly ConcurrentQueue<IntPtr> releasedVariables = new ConcurrentQueue<IntPtr>();
        private readonly ConcurrentQueue<IntPtr> releasedStates = new ConcurrentQueue<IntPtr>();

        // The states of the memory (destroyed before the memory)
        private readonly List<WeakReference<State>> states = new List<WeakReference<State>>();

        // Serializes the destruction of the memory and the releases that happen after it
        private readonly object sync = new object();

        private volatile bool disposed;

        /// <summary>
        /// Constructor of an empty memory
        /// </summary>
        /// <param name="gcThreshold">Number of objects from which the automatic garbage collector
        /// is executed (0 to disable the automatic garbage collector)</param>
        public Memory(long gcThreshold = 4096)
        {
            if (gcThreshold < 0)
                throw new ArgumentOutOfRangeException(nameof(gcThreshold));
            Native.EnsureInitialized();
            this.handle = NativeMethods.ls_memory_new((ulong)gcThreshold);
            if (this.handle == IntPtr.Zero)
                throw new OutOfMemoryException("can't create the memory");
        }

        /// <summary>
        /// Destructor of a memory that hasn't been disposed
        /// </summary>
        ~Memory()
        {
            this.Destroy();
        }

        /// <summary>
        /// Destroy the states and the objects of the memory (the variables that still exist remain
        /// valid but must not be used with a state)
        /// </summary>
        public void Dispose()
        {
            this.Destroy();
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Indicate if the memory has been disposed
        /// </summary>
        public bool IsDisposed => this.disposed;

        internal IntPtr Handle
        {
            get
            {
                if (this.disposed)
                    throw new ObjectDisposedException(nameof(Memory));
                this.ReleasePending();
                return this.handle;
            }
        }

        private void Destroy()
        {
            lock (this.sync)
            {
                if (this.disposed)
                    return;
                this.disposed = true;
                List<State> alive = new List<State>();
                lock (this.states)
                {
                    foreach (WeakReference<State> reference in this.states)
                    {
                        if (reference.TryGetTarget(out State? state))
                            alive.Add(state);
                    }
                    this.states.Clear();
                }
                foreach (State state in alive)
                    state.Destroy(false);
                while (this.releasedStates.TryDequeue(out IntPtr s))
                    NativeMethods.ls_state_delete(s);
                while (this.releasedVariables.TryDequeue(out IntPtr v))
                    NativeMethods.ls_var_delete(v);
                NativeMethods.ls_memory_delete(this.handle);
                this.handle = IntPtr.Zero;
            }
        }

        // Destroy the objects released by the finalizers
        internal void ReleasePending()
        {
            while (!this.releasedVariables.IsEmpty && this.releasedVariables.TryDequeue(out IntPtr v))
                NativeMethods.ls_var_delete(v);
            while (!this.releasedStates.IsEmpty && this.releasedStates.TryDequeue(out IntPtr s))
                NativeMethods.ls_state_delete(s);
        }

        // Release a native variable (finalizing: called by a finalizer)
        internal void ReleaseVariable(IntPtr variable, bool finalizing)
        {
            if (!finalizing && !this.disposed)
            {
                NativeMethods.ls_var_delete(variable);
                return;
            }
            lock (this.sync)
            {
                if (this.disposed)
                    NativeMethods.ls_var_delete(variable);
                else
                    this.releasedVariables.Enqueue(variable);
            }
        }

        // Release a native state (finalizing: called by a finalizer)
        internal void ReleaseState(IntPtr state, bool finalizing)
        {
            if (!finalizing && !this.disposed)
            {
                NativeMethods.ls_state_delete(state);
                return;
            }
            lock (this.sync)
            {
                if (this.disposed)
                    NativeMethods.ls_state_delete(state);
                else
                    this.releasedStates.Enqueue(state);
            }
        }

        internal void AddState(State state)
        {
            lock (this.states)
            {
                this.states.RemoveAll(r => !r.TryGetTarget(out _));
                this.states.Add(new WeakReference<State>(state));
            }
        }

        internal Variable Wrap(IntPtr variable)
        {
            return Variable.FromHandle(this, variable) ?? throw new InvalidOperationException("the engine returned an empty variable");
        }

        internal Variable? WrapOptional(IntPtr variable)
        {
            return Variable.FromHandle(this, variable);
        }

        internal void Check(int status)
        {
            Native.Check(status, this);
        }

        internal static IntPtr HandleOf(Variable? v)
        {
            if (v == null)
                return IntPtr.Zero;
            return v.Handle;
        }

        /// <summary>
        /// Return the number of objects in the memory
        /// </summary>
        public long Count => (long)NativeMethods.ls_memory_count(this.Handle);

        /// <summary>
        /// Destroy the objects that are not reachable from outside the memory
        /// </summary>
        /// <returns>The number of destroyed objects</returns>
        public long CollectGarbage()
        {
            this.Check(NativeMethods.ls_memory_collect_garbage(this.Handle, 0, out ulong count));
            return (long)count;
        }

        /// <summary>
        /// Execute the garbage collector if the number of objects reached the threshold
        /// (called by the states between instructions)
        /// </summary>
        /// <returns>The number of destroyed objects</returns>
        public long CollectGarbageIfNeeded()
        {
            this.Check(NativeMethods.ls_memory_collect_garbage(this.Handle, 1, out ulong count));
            return (long)count;
        }

        /// <summary>
        /// Change the threshold of the automatic garbage collector (0 to disable it)
        /// </summary>
        public void SetGarbageCollectorThreshold(long threshold)
        {
            if (threshold < 0)
                throw new ArgumentOutOfRangeException(nameof(threshold));
            NativeMethods.ls_memory_set_gc_threshold(this.Handle, (ulong)threshold);
        }

        /// <summary>
        /// Free the memory blocks kept by the current thread to create the next objects quickly
        /// (LiteScript::Object::ReleaseCachedBlocks)
        /// </summary>
        public static void ReleaseCachedBlocks()
        {
            Native.EnsureInitialized();
            NativeMethods.ls_release_cached_blocks();
        }

        /// <summary>
        /// Return the name of a type ("undefined", "null", "boolean"...)
        /// </summary>
        public static string GetTypeName(ScriptType type)
        {
            Native.EnsureInitialized();
            Native.Check(NativeMethods.ls_type_name((int)type, out IntPtr name));
            return Utf8.Take(name);
        }

        ////// OBJECTS CREATION //////

        public Variable CreateUndefined()
        {
            this.Check(NativeMethods.ls_memory_create_undefined(this.Handle, out IntPtr v));
            return this.Wrap(v);
        }

        public Variable CreateNull()
        {
            this.Check(NativeMethods.ls_memory_create_null(this.Handle, out IntPtr v));
            return this.Wrap(v);
        }

        public Variable CreateBoolean(bool value)
        {
            this.Check(NativeMethods.ls_memory_create_boolean(this.Handle, value ? 1 : 0, out IntPtr v));
            return this.Wrap(v);
        }

        public Variable CreateNumber(Number value)
        {
            NativeNumber n = value.ToNative();
            this.Check(NativeMethods.ls_memory_create_number(this.Handle, in n, out IntPtr v));
            return this.Wrap(v);
        }

        public Variable CreateString(string value)
        {
            byte[] bytes = Utf8.EncodeData(value, out UIntPtr length);
            this.Check(NativeMethods.ls_memory_create_string(this.Handle, bytes, length, out IntPtr v));
            return this.Wrap(v);
        }

        /// <summary>
        /// Create a character of a string object (assigning it modifies the string)
        /// </summary>
        public Variable CreateCharacter(Variable str, long index)
        {
            this.Check(NativeMethods.ls_memory_create_character(this.Handle, HandleOf(str), (ulong)index, out IntPtr v));
            GC.KeepAlive(str);
            return this.Wrap(v);
        }

        public Variable CreateNamespace()
        {
            this.Check(NativeMethods.ls_memory_create_namespace(this.Handle, out IntPtr v));
            return this.Wrap(v);
        }

        /// <summary>
        /// Create an empty class
        /// </summary>
        public Variable CreateClass()
        {
            this.Check(NativeMethods.ls_memory_create_class(this.Handle, out IntPtr v));
            return this.Wrap(v);
        }

        /// <summary>
        /// Create an instance of a class without calling its constructor
        /// </summary>
        public Variable CreateClassObject(Variable? baseClass)
        {
            this.Check(NativeMethods.ls_memory_create_instance(this.Handle, HandleOf(baseClass), out IntPtr v));
            GC.KeepAlive(baseClass);
            return this.Wrap(v);
        }

        /// <summary>
        /// Create an empty array
        /// </summary>
        public Variable CreateArray()
        {
            return this.CreateArray(Array.Empty<Variable?>());
        }

        /// <summary>
        /// Create an array that contains copies of the values
        /// </summary>
        public Variable CreateArray(params Variable?[] values)
        {
            return this.CreateArray((IEnumerable<Variable?>)values);
        }

        /// <summary>
        /// Create an array that contains copies of the values
        /// </summary>
        public Variable CreateArray(IEnumerable<Variable?> values)
        {
            var list = new List<Variable?>(values ?? throw new ArgumentNullException(nameof(values)));
            IntPtr[] handles = new IntPtr[list.Count];
            for (int i = 0; i < list.Count; i++)
                handles[i] = HandleOf(list[i]);
            this.Check(NativeMethods.ls_memory_create_array(this.Handle, handles, null, (UIntPtr)handles.Length, out IntPtr v));
            GC.KeepAlive(list);
            return this.Wrap(v);
        }

        /// <summary>
        /// Create an array with named elements that contains copies of the values
        /// </summary>
        public Variable CreateArray(params (string Name, Variable? Value)[] values)
        {
            var pairs = new List<KeyValuePair<string, Variable?>>();
            foreach (var (name, value) in values)
                pairs.Add(new KeyValuePair<string, Variable?>(name, value));
            return this.CreateArray(pairs);
        }

        /// <summary>
        /// Create an array with named elements that contains copies of the values
        /// </summary>
        public Variable CreateArray(IEnumerable<KeyValuePair<string, Variable?>> values)
        {
            var list = new List<KeyValuePair<string, Variable?>>(values ?? throw new ArgumentNullException(nameof(values)));
            IntPtr[] handles = new IntPtr[list.Count];
            IntPtr[] names = new IntPtr[list.Count];
            try
            {
                for (int i = 0; i < list.Count; i++)
                {
                    handles[i] = HandleOf(list[i].Value);
                    byte[] name = Utf8.Encode(list[i].Key);
                    names[i] = Marshal.AllocHGlobal(name.Length);
                    Marshal.Copy(name, 0, names[i], name.Length);
                }
                this.Check(NativeMethods.ls_memory_create_array(this.Handle, handles, names, (UIntPtr)handles.Length, out IntPtr v));
                GC.KeepAlive(list);
                return this.Wrap(v);
            }
            finally
            {
                foreach (IntPtr name in names)
                {
                    if (name != IntPtr.Zero)
                        Marshal.FreeHGlobal(name);
                }
            }
        }

        /// <summary>
        /// Create a callback that calls a function of the application
        /// (an unregistered function can't be saved with a state)
        /// </summary>
        public Variable CreateCallback(NativeFunction function)
        {
            if (function == null)
                throw new ArgumentNullException(nameof(function));
            IntPtr target = Native.Alloc(function);
            int status;
            IntPtr v;
            try
            {
                status = NativeMethods.ls_memory_create_callback(this.Handle, target, out v);
            }
            catch
            {
                GCHandle.FromIntPtr(target).Free();
                throw;
            }
            this.Check(status);
            return this.Wrap(v);
        }

        /// <summary>
        /// Create a callback that calls a registered function of the application
        /// </summary>
        /// <param name="name">The name of the registered function</param>
        /// <returns>The callback or an undefined object if the function is unknown</returns>
        public Variable CreateCallback(string name)
        {
            this.Check(NativeMethods.ls_memory_create_callback_named(this.Handle, Utf8.Encode(name), out IntPtr v));
            return this.Wrap(v);
        }

        ////// NATIVE FUNCTIONS //////

        /// <summary>
        /// Register a function of the application.
        /// The name is used to restore the callbacks when a state is loaded.
        /// </summary>
        /// <param name="name">The unique name of the function</param>
        /// <param name="function">The function</param>
        /// <returns>A callback that calls the function</returns>
        public Variable RegisterFunction(string name, NativeFunction function)
        {
            if (name == null)
                throw new ArgumentNullException(nameof(name));
            if (function == null)
                throw new ArgumentNullException(nameof(function));
            IntPtr target = Native.Alloc(function);
            int status;
            IntPtr v;
            try
            {
                status = NativeMethods.ls_memory_register_function(this.Handle, Utf8.Encode(name), target, out v);
            }
            catch
            {
                GCHandle.FromIntPtr(target).Free();
                throw;
            }
            this.Check(status);
            return this.Wrap(v);
        }

        /// <summary>
        /// Indicate if a function is registered (LiteScript::Memory::FindFunction)
        /// </summary>
        public bool HasFunction(string name)
        {
            return NativeMethods.ls_memory_has_function(this.Handle, Utf8.Encode(name)) != 0;
        }

        ////// METHODS OF THE TYPES //////

        /// <summary>
        /// Define the methods of the values of a type ("value.name(...)" calls the callback of the
        /// namespace with "this" bound to the value, the elements of an array have the priority)
        /// </summary>
        /// <param name="type">The type of the values</param>
        /// <param name="namespaceObject">A namespace of callbacks (null removes the methods)</param>
        public void SetMethods(ScriptType type, Variable? namespaceObject)
        {
            this.Check(NativeMethods.ls_memory_set_methods(this.Handle, (int)type, HandleOf(namespaceObject)));
            GC.KeepAlive(namespaceObject);
        }

        /// <summary>
        /// Find a method of a type
        /// </summary>
        /// <returns>The callback or null if not found</returns>
        public Variable? FindMethod(ScriptType type, string name)
        {
            this.Check(NativeMethods.ls_memory_find_method(this.Handle, (int)type, Utf8.Encode(name), out IntPtr v));
            return this.WrapOptional(v);
        }
    }
}
