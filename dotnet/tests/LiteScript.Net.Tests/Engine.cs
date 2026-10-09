using System;
using System.Collections.Generic;
using System.Text;

namespace LiteScript.Tests
{
    // A memory and a state with a "print" function that writes in Output
    internal sealed class Engine : IDisposable
    {
        public readonly Memory Memory = new Memory();
        public readonly State State;
        private readonly List<string> output = new List<string>();

        public Engine()
        {
            this.State = new State(this.Memory);
            this.State.DeclareVariable("print", this.Memory.RegisterFunction("print", (state, args) =>
            {
                var line = new StringBuilder();
                for (int i = 0; i < args.Count; i++)
                {
                    if (i > 0)
                        line.Append(", ");
                    line.Append(Operations.ToText(args[i]));
                }
                this.output.Add(line.ToString());
                return null;
            }));
        }

        public string Output => string.Join("\n", this.output);

        /// <summary>
        /// Execute a code that must compile, return the value of its "return"
        /// </summary>
        public Variable? Run(string code)
        {
            Script.Result result = Script.Execute(this.State, code);
            if (!result.Success)
                throw new InvalidOperationException($"{result.Line + 1}:{result.Column + 1} {result.GetMessage()}");
            return result.Value;
        }

        /// <summary>
        /// Return the description of the value of an expression
        /// </summary>
        public string Eval(string expression)
        {
            return this.Run("return " + expression + ";")?.ToString() ?? "<empty>";
        }

        public void Dispose()
        {
            this.State.Dispose();
            this.Memory.Dispose();
        }
    }
}
