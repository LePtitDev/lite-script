using System;
using System.Collections;
using System.Collections.Generic;
using LiteScript.Interop;

namespace LiteScript
{
    /// <summary>
    /// A list of variables of a value: the indexed elements of an array or the inherited classes of a class
    /// (std::vector&lt;Variable&gt;). The items are new references to the objects, null for an empty variable.
    /// </summary>
    public sealed class VariableList : IList<Variable?>, IReadOnlyList<Variable?>
    {
        private readonly Variable owner;
        private readonly int kind;

        internal VariableList(Variable owner, int kind)
        {
            this.owner = owner;
            this.kind = kind;
        }

        private Memory Memory => this.owner.Memory;

        public int Count
        {
            get
            {
                Native.Check(NativeMethods.ls_vector_count(this.owner.Handle, this.kind, out ulong count));
                GC.KeepAlive(this.owner);
                return checked((int)count);
            }
        }

        public bool IsReadOnly => false;

        public Variable? this[int index]
        {
            get
            {
                if (index < 0)
                    throw new ArgumentOutOfRangeException(nameof(index));
                this.Memory.Check(NativeMethods.ls_vector_get(this.owner.Handle, this.kind, (ulong)index, out IntPtr v));
                GC.KeepAlive(this.owner);
                return this.Memory.WrapOptional(v);
            }
            set
            {
                if (index < 0)
                    throw new ArgumentOutOfRangeException(nameof(index));
                this.Memory.Check(NativeMethods.ls_vector_set(this.owner.Handle, this.kind, (ulong)index, Memory.HandleOf(value)));
                GC.KeepAlive(this.owner);
                GC.KeepAlive(value);
            }
        }

        /// <summary>
        /// Add a variable at the end (the variable is shared, use <see cref="Operations.Copy"/> to add a copy)
        /// </summary>
        public void Add(Variable? item) => this.Insert(this.Count, item);

        public void Insert(int index, Variable? item)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            this.Memory.Check(NativeMethods.ls_vector_insert(this.owner.Handle, this.kind, (ulong)index, Memory.HandleOf(item)));
            GC.KeepAlive(this.owner);
            GC.KeepAlive(item);
        }

        public void RemoveAt(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            this.Memory.Check(NativeMethods.ls_vector_remove(this.owner.Handle, this.kind, (ulong)index));
            GC.KeepAlive(this.owner);
        }

        public void Clear()
        {
            this.Memory.Check(NativeMethods.ls_vector_clear(this.owner.Handle, this.kind));
            GC.KeepAlive(this.owner);
        }

        /// <summary>
        /// Return the index of the first item that references the same object (-1 if not found)
        /// </summary>
        public int IndexOf(Variable? item)
        {
            for (int i = 0, count = this.Count; i < count; i++)
            {
                using (Variable? v = this[i])
                {
                    if (Variable.SameObject(v, item))
                        return i;
                }
            }
            return -1;
        }

        public bool Contains(Variable? item) => this.IndexOf(item) >= 0;

        public bool Remove(Variable? item)
        {
            int index = this.IndexOf(item);
            if (index < 0)
                return false;
            this.RemoveAt(index);
            return true;
        }

        public void CopyTo(Variable?[] array, int arrayIndex)
        {
            for (int i = 0, count = this.Count; i < count; i++)
                array[arrayIndex + i] = this[i];
        }

        public IEnumerator<Variable?> GetEnumerator()
        {
            for (int i = 0; i < this.Count; i++)
                yield return this[i];
        }

        IEnumerator IEnumerable.GetEnumerator() => this.GetEnumerator();
    }

    /// <summary>
    /// An ordered list of named variables of a value (LiteScript::MemberList): the named elements of
    /// an array, the members of a class or an instance, the variables of a namespace.
    /// </summary>
    public sealed class MemberList : IReadOnlyList<KeyValuePair<string, Variable?>>
    {
        private readonly Variable owner;
        private readonly int kind;

        internal MemberList(Variable owner, int kind)
        {
            this.owner = owner;
            this.kind = kind;
        }

        private Memory Memory => this.owner.Memory;

        public int Count
        {
            get
            {
                Native.Check(NativeMethods.ls_members_count(this.owner.Handle, this.kind, out ulong count));
                GC.KeepAlive(this.owner);
                return checked((int)count);
            }
        }

        public bool Empty => this.Count == 0;

        public string GetName(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            Native.Check(NativeMethods.ls_members_name(this.owner.Handle, this.kind, (ulong)index, out IntPtr name));
            GC.KeepAlive(this.owner);
            return Utf8.Take(name);
        }

        public Variable? GetValue(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            this.Memory.Check(NativeMethods.ls_members_value(this.owner.Handle, this.kind, (ulong)index, out IntPtr v));
            GC.KeepAlive(this.owner);
            return this.Memory.WrapOptional(v);
        }

        public KeyValuePair<string, Variable?> this[int index] => new KeyValuePair<string, Variable?>(this.GetName(index), this.GetValue(index));

        /// <summary>
        /// The variable of a member (get: null if not found, set: defines the member)
        /// </summary>
        public Variable? this[string name]
        {
            get => this.Find(name);
            set => this.Set(name, value);
        }

        /// <summary>
        /// Return the index of a member (or -1 if not found)
        /// </summary>
        public int IndexOf(string name)
        {
            Native.Check(NativeMethods.ls_members_index_of(this.owner.Handle, this.kind, Utf8.Encode(name), out long index));
            GC.KeepAlive(this.owner);
            return checked((int)index);
        }

        /// <summary>
        /// Return the variable of a member (or null if not found)
        /// </summary>
        public Variable? Find(string name)
        {
            this.Memory.Check(NativeMethods.ls_members_find(this.owner.Handle, this.kind, Utf8.Encode(name), out IntPtr v));
            GC.KeepAlive(this.owner);
            return this.Memory.WrapOptional(v);
        }

        /// <summary>
        /// Define a member, replace the previous one with the same name
        /// (the variable is shared, use <see cref="Operations.Copy"/> to set a copy)
        /// </summary>
        public void Set(string name, Variable? value)
        {
            this.Memory.Check(NativeMethods.ls_members_set(this.owner.Handle, this.kind, Utf8.Encode(name), Memory.HandleOf(value)));
            GC.KeepAlive(this.owner);
            GC.KeepAlive(value);
        }

        public IEnumerator<KeyValuePair<string, Variable?>> GetEnumerator()
        {
            for (int i = 0; i < this.Count; i++)
                yield return this[i];
        }

        IEnumerator IEnumerable.GetEnumerator() => this.GetEnumerator();
    }

    /// <summary>
    /// Content of an associative array (LiteScript::Array)
    /// </summary>
    public sealed class ArrayValue
    {
        internal ArrayValue(Variable owner)
        {
            this.Indexed = new VariableList(owner, NativeMethods.VectorArrayIndexed);
            this.Named = new MemberList(owner, NativeMethods.MembersArrayNamed);
        }

        /// <summary>
        /// The elements indexed by numbers
        /// </summary>
        public VariableList Indexed { get; }

        /// <summary>
        /// The elements indexed by names
        /// </summary>
        public MemberList Named { get; }
    }

    /// <summary>
    /// Content of a class (LiteScript::Class)
    /// </summary>
    public sealed class ClassValue
    {
        private readonly Variable owner;

        internal ClassValue(Variable owner)
        {
            this.owner = owner;
            this.Inherits = new VariableList(owner, NativeMethods.VectorClassInherits);
            this.Statics = new MemberList(owner, NativeMethods.MembersClassStatics);
            this.Members = new MemberList(owner, NativeMethods.MembersClassMembers);
        }

        private Memory Memory => this.owner.Memory;

        /// <summary>
        /// The inherited classes
        /// </summary>
        public VariableList Inherits { get; }

        /// <summary>
        /// The static members
        /// </summary>
        public MemberList Statics { get; }

        /// <summary>
        /// The members of the instances (callbacks are methods)
        /// </summary>
        public MemberList Members { get; }

        /// <summary>
        /// The name of the constructor in members (empty if there is no constructor)
        /// </summary>
        public string Constructor
        {
            get
            {
                Native.Check(NativeMethods.ls_class_get_constructor_name(this.owner.Handle, out IntPtr name));
                GC.KeepAlive(this.owner);
                return Utf8.Take(name);
            }
            set
            {
                Native.Check(NativeMethods.ls_class_set_constructor_name(this.owner.Handle, Utf8.Encode(value)));
                GC.KeepAlive(this.owner);
            }
        }

        /// <summary>
        /// Inherit the members and operators of a class
        /// </summary>
        /// <param name="parent">The class object to inherit</param>
        public void Inherit(Variable parent)
        {
            this.Memory.Check(NativeMethods.ls_class_inherit(this.owner.Handle, Memory.HandleOf(parent)));
            GC.KeepAlive(this.owner);
            GC.KeepAlive(parent);
        }

        /// <summary>
        /// Define the constructor by its name
        /// </summary>
        /// <returns>false if the member doesn't exist or isn't a callback</returns>
        public bool DefineConstructor(string name)
        {
            this.Memory.Check(NativeMethods.ls_class_define_constructor(this.owner.Handle, Utf8.Encode(name), out int defined));
            GC.KeepAlive(this.owner);
            return defined != 0;
        }

        /// <summary>
        /// Return the constructor (null if there is no constructor)
        /// </summary>
        public Variable? GetConstructor()
        {
            this.Memory.Check(NativeMethods.ls_class_get_constructor(this.owner.Handle, out IntPtr v));
            GC.KeepAlive(this.owner);
            return this.Memory.WrapOptional(v);
        }

        /// <summary>
        /// Return the overloaded operator (null if not defined)
        /// </summary>
        public Variable? GetOperator(Operator op)
        {
            this.Memory.Check(NativeMethods.ls_class_get_operator(this.owner.Handle, (int)op, out IntPtr v));
            GC.KeepAlive(this.owner);
            return this.Memory.WrapOptional(v);
        }

        /// <summary>
        /// Define an overloaded operator (null removes it)
        /// </summary>
        public void SetOperator(Operator op, Variable? callback)
        {
            this.Memory.Check(NativeMethods.ls_class_set_operator(this.owner.Handle, (int)op, Memory.HandleOf(callback)));
            GC.KeepAlive(this.owner);
            GC.KeepAlive(callback);
        }
    }

    /// <summary>
    /// Content of an instance of class (LiteScript::ClassObject)
    /// </summary>
    public sealed class ClassObjectValue
    {
        private readonly Variable owner;

        internal ClassObjectValue(Variable owner)
        {
            this.owner = owner;
            this.Members = new MemberList(owner, NativeMethods.MembersInstanceMembers);
        }

        /// <summary>
        /// The class of the instance
        /// </summary>
        public Variable? Base
        {
            get
            {
                this.owner.Memory.Check(NativeMethods.ls_instance_get_base(this.owner.Handle, out IntPtr v));
                GC.KeepAlive(this.owner);
                return this.owner.Memory.WrapOptional(v);
            }
            set
            {
                this.owner.Memory.Check(NativeMethods.ls_instance_set_base(this.owner.Handle, Memory.HandleOf(value)));
                GC.KeepAlive(this.owner);
                GC.KeepAlive(value);
            }
        }

        /// <summary>
        /// The attributes
        /// </summary>
        public MemberList Members { get; }
    }

    /// <summary>
    /// Content of a namespace (LiteScript::Namespace)
    /// </summary>
    public sealed class NamespaceValue
    {
        internal NamespaceValue(Variable owner)
        {
            this.Variables = new MemberList(owner, NativeMethods.MembersNamespaceVariables);
        }

        /// <summary>
        /// The variables
        /// </summary>
        public MemberList Variables { get; }
    }

    /// <summary>
    /// A function of the script (LiteScript::ScriptFunction)
    /// </summary>
    public sealed class ScriptFunction
    {
        internal ScriptFunction(int list, int line, Namer scope)
        {
            this.List = list;
            this.Line = line;
            this.Scope = scope;
        }

        /// <summary>
        /// The index of the instructions list
        /// </summary>
        public int List { get; }

        /// <summary>
        /// The index of the first instruction
        /// </summary>
        public int Line { get; }

        /// <summary>
        /// The namespaces visible by the function
        /// </summary>
        public Namer Scope { get; }
    }

    /// <summary>
    /// Content of a function (LiteScript::Callback)
    /// </summary>
    public sealed class CallbackValue
    {
        private readonly Variable owner;

        internal CallbackValue(Variable owner)
        {
            this.owner = owner;
        }

        /// <summary>
        /// Indicate if the callback calls a function of the application
        /// </summary>
        public bool IsNative
        {
            get
            {
                Native.Check(NativeMethods.ls_callback_is_native(this.owner.Handle, out int native));
                GC.KeepAlive(this.owner);
                return native != 0;
            }
        }

        /// <summary>
        /// The name of the registered function of the application (empty for an unregistered function or a script function)
        /// </summary>
        public string NativeName
        {
            get
            {
                Native.Check(NativeMethods.ls_callback_native_name(this.owner.Handle, out IntPtr name));
                GC.KeepAlive(this.owner);
                return Utf8.Take(name);
            }
        }

        /// <summary>
        /// The function of the script (null for a function of the application)
        /// </summary>
        public ScriptFunction? ScriptFunction
        {
            get
            {
                if (this.IsNative)
                    return null;
                Memory memory = this.owner.Memory;
                memory.Check(NativeMethods.ls_callback_script_function(this.owner.Handle, out ulong list, out ulong line, out IntPtr buffer, out UIntPtr size));
                GC.KeepAlive(this.owner);
                var reader = new BufferReader(buffer, size, memory);
                return new ScriptFunction(checked((int)list), checked((int)line), reader.Namer());
            }
        }

        /// <summary>
        /// The value of "this" (null if not bound)
        /// </summary>
        public Variable? Self
        {
            get
            {
                this.owner.Memory.Check(NativeMethods.ls_callback_get_self(this.owner.Handle, out IntPtr v));
                GC.KeepAlive(this.owner);
                return this.owner.Memory.WrapOptional(v);
            }
            set
            {
                this.owner.Memory.Check(NativeMethods.ls_callback_set_self(this.owner.Handle, Memory.HandleOf(value)));
                GC.KeepAlive(this.owner);
                GC.KeepAlive(value);
            }
        }

        /// <summary>
        /// Indicate if two callbacks call the same function
        /// </summary>
        public bool SameTarget(CallbackValue other)
        {
            if (other == null)
                throw new ArgumentNullException(nameof(other));
            this.owner.Memory.Check(NativeMethods.ls_callback_same_target(this.owner.Handle, other.owner.Handle, out int same));
            GC.KeepAlive(this.owner);
            GC.KeepAlive(other.owner);
            return same != 0;
        }
    }

    /// <summary>
    /// A character of a string object (LiteScript::Character, the result of "string[index]"),
    /// assigning it modifies the string
    /// </summary>
    public sealed class CharacterValue
    {
        private readonly Variable owner;

        internal CharacterValue(Variable owner)
        {
            this.owner = owner;
        }

        private void Get(out Variable? str, out long index)
        {
            this.owner.Memory.Check(NativeMethods.ls_character_get(this.owner.Handle, out IntPtr s, out ulong i));
            GC.KeepAlive(this.owner);
            str = this.owner.Memory.WrapOptional(s);
            index = (long)i;
        }

        /// <summary>
        /// The string object
        /// </summary>
        public Variable? String
        {
            get
            {
                this.Get(out Variable? str, out _);
                return str;
            }
            set
            {
                this.owner.Memory.Check(NativeMethods.ls_character_set(this.owner.Handle, Memory.HandleOf(value), (ulong)this.Index));
                GC.KeepAlive(this.owner);
                GC.KeepAlive(value);
            }
        }

        /// <summary>
        /// The index of the character in the string
        /// </summary>
        public long Index
        {
            get
            {
                this.Get(out Variable? str, out long index);
                str?.Dispose();
                return index;
            }
            set
            {
                if (value < 0)
                    throw new ArgumentOutOfRangeException(nameof(value));
                using (Variable? str = this.String)
                {
                    this.owner.Memory.Check(NativeMethods.ls_character_set(this.owner.Handle, Memory.HandleOf(str), (ulong)value));
                    GC.KeepAlive(this.owner);
                }
            }
        }
    }
}
