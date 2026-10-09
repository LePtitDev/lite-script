using System;

namespace LiteScript
{
    /// <summary>
    /// An error of a script that hasn't been caught by a "try ... catch" block (LiteScript::ScriptError):
    /// raised by "throw", by an invalid operation or by an exception of a native function.
    /// A native function can throw it to raise an error with a value.
    /// </summary>
    public class ScriptException : Exception
    {
        /// <param name="message">The message</param>
        /// <param name="value">The thrown value</param>
        /// <param name="position">The position of the instruction that raised the error</param>
        public ScriptException(string message, Variable? value = null, SourcePosition position = default) : base(message)
        {
            this.Value = value;
            this.Position = position;
        }

        /// <summary>
        /// Create an error that throws a value (the message is the text of the value)
        /// </summary>
        public ScriptException(Variable value) : this(value?.ToText() ?? "undefined", value)
        {
        }

        /// <summary>
        /// The thrown value (for the errors of the engine: an array with the members "message",
        /// "line" and "column", the line and the column start at 1)
        /// </summary>
        public Variable? Value { get; }

        /// <summary>
        /// The position of the instruction that raised the error (starts at 0)
        /// </summary>
        public SourcePosition Position { get; }
    }

    /// <summary>
    /// Error raised when a binary stream can't be written or read (LiteScript::SerializationError)
    /// </summary>
    public class SerializationException : Exception
    {
        public SerializationException(string message) : base(message)
        {
        }
    }
}
