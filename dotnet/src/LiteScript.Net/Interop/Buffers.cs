using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;

namespace LiteScript.Interop
{
    // Kinds of operands of the instructions in the buffers
    internal static class OperandKind
    {
        public const byte None = 0;
        public const byte Boolean = 1;
        public const byte Integer = 2;
        public const byte Float = 3;
        public const byte String = 4;
    }

    // Writer of a little endian buffer given to the native library
    internal sealed class BufferWriter
    {
        private readonly MemoryStream stream = new MemoryStream();
        private readonly BinaryWriter writer;

        // The variables written in the buffer (kept alive until the buffer is used)
        private readonly List<Variable> variables = new List<Variable>();

        public BufferWriter()
        {
            this.writer = new BinaryWriter(this.stream);
        }

        public void U8(byte v) => this.writer.Write(v);
        public void U32(uint v) => this.writer.Write(v);
        public void I64(long v) => this.writer.Write(v);
        public void F64(double v) => this.writer.Write(v);

        public void Str(string s)
        {
            byte[] bytes = Utf8.EncodeData(s, out _);
            this.writer.Write((uint)bytes.Length);
            this.writer.Write(bytes);
        }

        // A borrowed handle (the variable must stay alive until the buffer is used, see KeepAlive)
        public void Var(Variable? v)
        {
            if (v == null)
            {
                this.writer.Write(0UL);
                return;
            }
            this.variables.Add(v);
            this.writer.Write((ulong)v.Handle.ToInt64());
        }

        public void Instruction(Instruction instruction)
        {
            this.U8((byte)instruction.Code);
            switch (instruction.Operand)
            {
                case bool b:
                    this.U8(OperandKind.Boolean);
                    this.U8(b ? (byte)1 : (byte)0);
                    break;
                case long i:
                    this.U8(OperandKind.Integer);
                    this.I64(i);
                    break;
                case double d:
                    this.U8(OperandKind.Float);
                    this.F64(d);
                    break;
                case string s:
                    this.U8(OperandKind.String);
                    this.Str(s);
                    break;
                default:
                    this.U8(OperandKind.None);
                    break;
            }
            this.U32(instruction.Position.Line);
            this.U32(instruction.Position.Column);
        }

        public void Instructions(IEnumerable<Instruction> instructions)
        {
            if (instructions == null)
                throw new ArgumentNullException(nameof(instructions));
            var list = instructions as ICollection<Instruction> ?? new List<Instruction>(instructions);
            this.U32((uint)list.Count);
            foreach (Instruction instruction in list)
                this.Instruction(instruction ?? throw new ArgumentException("an instruction is null", nameof(instructions)));
        }

        public void Namer(Namer namer)
        {
            this.Var(namer.Global);
            this.Var(namer.Current);
            this.U32((uint)namer.Scopes.Count);
            foreach (Variable? scope in namer.Scopes)
                this.Var(scope);
        }

        public byte[] ToArray()
        {
            this.writer.Flush();
            return this.stream.ToArray();
        }

        public void KeepAlive()
        {
            GC.KeepAlive(this.variables);
            foreach (Variable v in this.variables)
                GC.KeepAlive(v);
        }

        public static byte[] FromInstructions(IEnumerable<Instruction> instructions)
        {
            var w = new BufferWriter();
            w.Instructions(instructions);
            return w.ToArray();
        }

        public static byte[] FromInstruction(Instruction instruction)
        {
            if (instruction == null)
                throw new ArgumentNullException(nameof(instruction));
            var w = new BufferWriter();
            w.Instruction(instruction);
            return w.ToArray();
        }
    }

    // Reader of a little endian buffer returned by the native library
    internal sealed class BufferReader
    {
        private readonly BinaryReader reader;
        private readonly Memory? memory;

        // Copy a buffer allocated by the native library and release it
        public BufferReader(IntPtr buffer, UIntPtr size, Memory? memory = null)
        {
            byte[] data = Take(buffer, size);
            this.reader = new BinaryReader(new MemoryStream(data));
            this.memory = memory;
        }

        public static byte[] Take(IntPtr buffer, UIntPtr size)
        {
            try
            {
                byte[] data = new byte[(int)size.ToUInt64()];
                if (data.Length > 0)
                    Marshal.Copy(buffer, data, 0, data.Length);
                return data;
            }
            finally
            {
                if (buffer != IntPtr.Zero)
                    NativeMethods.ls_free(buffer);
            }
        }

        public byte U8() => this.reader.ReadByte();
        public uint U32() => this.reader.ReadUInt32();
        public ulong U64() => this.reader.ReadUInt64();
        public long I64() => this.reader.ReadInt64();
        public double F64() => this.reader.ReadDouble();

        // An index or a size (-1 for "none")
        public int Index()
        {
            ulong v = this.reader.ReadUInt64();
            return v == ulong.MaxValue ? -1 : checked((int)v);
        }

        public string Str()
        {
            int length = checked((int)this.reader.ReadUInt32());
            return Utf8.GetString(this.reader.ReadBytes(length));
        }

        // A new handle owned by the reader
        public Variable? Var()
        {
            IntPtr handle = new IntPtr((long)this.reader.ReadUInt64());
            if (this.memory == null)
            {
                if (handle != IntPtr.Zero)
                    NativeMethods.ls_var_delete(handle);
                throw new InvalidOperationException("no memory to read a variable");
            }
            return Variable.FromHandle(this.memory, handle);
        }

        public Instruction Instruction()
        {
            var code = (InstrCode)this.U8();
            object? operand;
            switch (this.U8())
            {
                case OperandKind.Boolean: operand = this.U8() != 0; break;
                case OperandKind.Integer: operand = this.I64(); break;
                case OperandKind.Float: operand = this.F64(); break;
                case OperandKind.String: operand = this.Str(); break;
                default: operand = null; break;
            }
            uint line = this.U32();
            uint column = this.U32();
            return new Instruction(code, operand) { Position = new SourcePosition(line, column) };
        }

        public List<Instruction> Instructions()
        {
            int count = checked((int)this.U32());
            var list = new List<Instruction>(count);
            for (int i = 0; i < count; i++)
                list.Add(this.Instruction());
            return list;
        }

        public List<Variable?> Variables()
        {
            int count = checked((int)this.U32());
            var list = new List<Variable?>(count);
            for (int i = 0; i < count; i++)
                list.Add(this.Var());
            return list;
        }

        public Namer Namer()
        {
            var namer = new Namer(this.memory!)
            {
                Global = this.Var(),
                Current = this.Var()
            };
            namer.Scopes.AddRange(this.Variables());
            return namer;
        }
    }
}
