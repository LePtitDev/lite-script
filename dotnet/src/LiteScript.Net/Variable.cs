using System;
using System.Threading;
using LiteScript.Interop;

namespace LiteScript
{
    /// <summary>
    /// A counted reference to an object of a memory (LiteScript::Variable).
    /// Copying the reference with <see cref="Duplicate"/> shares the object, the object is destroyed
    /// when its last reference disappears. An empty C++ variable is represented by null.
    /// Disposing a variable releases its reference immediately, otherwise it's released after the
    /// .NET garbage collection (by the next operation of its memory).
    /// </summary>
    public sealed class Variable : IDisposable, IEquatable<Variable>
    {
        // The native variable (zero when disposed)
        private IntPtr handle;

        private Variable(Memory memory, IntPtr handle)
        {
            this.Memory = memory;
            this.handle = handle;
        }

        internal static Variable? FromHandle(Memory memory, IntPtr handle)
        {
            return handle == IntPtr.Zero ? null : new Variable(memory, handle);
        }

        /// <summary>
        /// Destructor of a variable that hasn't been disposed
        /// </summary>
        ~Variable()
        {
            this.Release(true);
        }

        /// <summary>
        /// Release the reference
        /// </summary>
        public void Dispose()
        {
            this.Release(false);
            GC.SuppressFinalize(this);
        }

        private void Release(bool finalizing)
        {
            IntPtr h = Interlocked.Exchange(ref this.handle, IntPtr.Zero);
            if (h != IntPtr.Zero)
                this.Memory.ReleaseVariable(h, finalizing);
        }

        /// <summary>
        /// The memory that contains the object
        /// </summary>
        public Memory Memory { get; }

        /// <summary>
        /// Indicate if the reference has been released
        /// </summary>
        public bool IsDisposed => this.handle == IntPtr.Zero;

        internal IntPtr Handle
        {
            get
            {
                IntPtr h = this.handle;
                if (h == IntPtr.Zero)
                    throw new ObjectDisposedException(nameof(Variable));
                if (this.Memory.IsDisposed)
                    throw new ObjectDisposedException(nameof(LiteScript.Memory));
                this.Memory.ReleasePending();
                return h;
            }
        }

        private T Use<T>(Func<IntPtr, T> function)
        {
            try
            {
                return function(this.Handle);
            }
            finally
            {
                GC.KeepAlive(this);
            }
        }

        /// <summary>
        /// Create a new reference to the same object
        /// </summary>
        public Variable Duplicate()
        {
            return this.Memory.Wrap(this.Use(NativeMethods.ls_var_duplicate));
        }

        /// <summary>
        /// Return the type of the referenced object (the type of the target for a reference)
        /// </summary>
        public ScriptType Type => (ScriptType)this.Use(NativeMethods.ls_var_type);

        /// <summary>
        /// Indicate if the referenced object contains a value of a type
        /// </summary>
        public bool Is(ScriptType type) => this.Type == type;

        /// <summary>
        /// Return the number of variables that reference the object
        /// </summary>
        public long ReferenceCount => (long)this.Use(NativeMethods.ls_var_reference_count);

        /// <summary>
        /// Indicate if the object contains a reference to another object (arrays, instances, classes
        /// and namespaces are shared by assignment: the variables contain a reference to them)
        /// </summary>
        public bool IsReference => this.Use(NativeMethods.ls_var_is_reference) != 0;

        /// <summary>
        /// Return the object that contains the value (the referenced object for a reference, the same object otherwise)
        /// </summary>
        public Variable Target() => this.Memory.Wrap(this.Use(NativeMethods.ls_var_target));

        ////// VALUES //////

        private void Expect(ScriptType type, string name)
        {
            if (this.Type != type)
                throw new InvalidCastException("the value isn't " + name);
        }

        /// <summary>
        /// Return the value of a boolean
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't a boolean</exception>
        public bool AsBoolean()
        {
            this.Expect(ScriptType.Boolean, "a boolean");
            Native.Check(NativeMethods.ls_var_get_boolean(this.Handle, out int value));
            GC.KeepAlive(this);
            return value != 0;
        }

        /// <summary>
        /// Return the value of a number
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't a number</exception>
        public Number AsNumber()
        {
            this.Expect(ScriptType.Number, "a number");
            Native.Check(NativeMethods.ls_var_get_number(this.Handle, out NativeNumber value));
            GC.KeepAlive(this);
            return new Number(in value);
        }

        /// <summary>
        /// Return the content of a string
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't a string</exception>
        public string AsString()
        {
            this.Expect(ScriptType.String, "a string");
            Native.Check(NativeMethods.ls_var_get_string(this.Handle, out IntPtr text));
            GC.KeepAlive(this);
            return Utf8.Take(text);
        }

        /// <summary>
        /// Return the number of code points of a string
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't a string</exception>
        public long GetStringLength()
        {
            this.Expect(ScriptType.String, "a string");
            Native.Check(NativeMethods.ls_var_get_string_length(this.Handle, out ulong length));
            GC.KeepAlive(this);
            return (long)length;
        }

        /// <summary>
        /// Return the content of an array
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't an array</exception>
        public ArrayValue AsArray()
        {
            this.Expect(ScriptType.Array, "an array");
            return new ArrayValue(this);
        }

        /// <summary>
        /// Return the content of a class
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't a class</exception>
        public ClassValue AsClass()
        {
            this.Expect(ScriptType.Class, "a class");
            return new ClassValue(this);
        }

        /// <summary>
        /// Return the content of an instance of class
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't an instance of class</exception>
        public ClassObjectValue AsClassObject()
        {
            this.Expect(ScriptType.ClassObject, "an instance of class");
            return new ClassObjectValue(this);
        }

        /// <summary>
        /// Return the content of a namespace
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't a namespace</exception>
        public NamespaceValue AsNamespace()
        {
            this.Expect(ScriptType.Namespace, "a namespace");
            return new NamespaceValue(this);
        }

        /// <summary>
        /// Return the content of a callback
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't a callback</exception>
        public CallbackValue AsCallback()
        {
            this.Expect(ScriptType.Callback, "a callback");
            return new CallbackValue(this);
        }

        /// <summary>
        /// Return the content of a character of a string
        /// </summary>
        /// <exception cref="InvalidCastException">The value isn't a character</exception>
        public CharacterValue AsCharacter()
        {
            this.Expect(ScriptType.Character, "a character");
            return new CharacterValue(this);
        }

        ////// CONTENT REPLACEMENT //////

        private void Set(int kind, bool boolean, Number number, string? text)
        {
            NativeNumber n = number.ToNative();
            byte[]? bytes = text != null ? Utf8.EncodeData(text, out _) : null;
            Native.Check(NativeMethods.ls_var_set(this.Handle, kind, boolean ? 1 : 0, in n, bytes, (UIntPtr)(bytes?.Length ?? 0)));
            GC.KeepAlive(this);
        }

        /// <summary>
        /// Replace the content of the object by undefined
        /// </summary>
        public void SetUndefined() => this.Set(0, false, default, null);

        /// <summary>
        /// Replace the content of the object by null
        /// </summary>
        public void SetNull() => this.Set(1, false, default, null);

        /// <summary>
        /// Replace the content of the object by a boolean
        /// </summary>
        public void SetBoolean(bool value) => this.Set(2, value, default, null);

        /// <summary>
        /// Replace the content of the object by a number
        /// </summary>
        public void SetNumber(Number value) => this.Set(3, false, value, null);

        /// <summary>
        /// Replace the content of the object by a string
        /// </summary>
        public void SetString(string value) => this.Set(4, false, default, value ?? throw new ArgumentNullException(nameof(value)));

        ////// TEXTS //////

        /// <summary>
        /// Return a string that describes the content of the object (strings are quoted)
        /// </summary>
        public override string ToString()
        {
            if (this.IsDisposed)
                return "<disposed>";
            Native.Check(NativeMethods.ls_var_to_string(this.Handle, out IntPtr text));
            GC.KeepAlive(this);
            return Utf8.Take(text);
        }

        /// <summary>
        /// Return the text of the value: the content of strings and characters, the description of the other types
        /// </summary>
        public string ToText()
        {
            Native.Check(NativeMethods.ls_var_to_text(this.Handle, out IntPtr text));
            GC.KeepAlive(this);
            return Utf8.Take(text);
        }

        ////// IDENTITY //////

        /// <summary>
        /// Indicate if two variables reference the same object
        /// </summary>
        public bool Equals(Variable? other)
        {
            if (other is null)
                return false;
            if (ReferenceEquals(this, other))
                return true;
            bool same = NativeMethods.ls_var_same(this.Handle, other.Handle) != 0;
            GC.KeepAlive(this);
            GC.KeepAlive(other);
            return same;
        }

        /// <inheritdoc/>
        public override bool Equals(object? obj) => this.Equals(obj as Variable);

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            if (this.IsDisposed)
                return 0;
            return this.Use(NativeMethods.ls_var_object_id).GetHashCode();
        }

        /// <summary>
        /// Indicate if two variables reference the same object (two nulls are the same empty variable)
        /// </summary>
        public static bool SameObject(Variable? a, Variable? b)
        {
            if (a is null || b is null)
                return a is null && b is null;
            return a.Equals(b);
        }
    }
}
