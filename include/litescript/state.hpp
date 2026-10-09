#ifndef LITESCRIPT_STATE_HPP
#define LITESCRIPT_STATE_HPP

#include <cstddef>
#include <iosfwd>
#include <string_view>
#include <vector>

#include "error.hpp"
#include "instruction.hpp"
#include "memory.hpp"
#include "operations.hpp"

namespace LiteScript {

    // The execution state of a script: instructions, position, stacks and namespaces
    // The memory must outlive the state.
    class State {

        // Kinds of frames
        enum class FrameKind : std::uint8_t {
            // A scope pushed by the "push.nsp" instruction
            Scope,
            // A call of a script function, the result is pushed on the stack
            Call,
            // A call of a constructor, the result is ignored
            Constructor,
            // A call made by the application with State::Call
            Host,
            // A call of a native function
            Native
        };

        // A frame of the execution (a function call or a scope)
        struct Frame {
            FrameKind kind = FrameKind::Scope;
            // The namespaces resolution
            Namer namer;
            // The value of "this"
            Variable self;
            // The return value
            Variable result;
            // The arguments
            std::vector<Variable> args;
            // The position to restore when the function returns
            std::size_t return_list = 0;
            std::size_t return_line = 0;
            // The sizes of the stacks to restore when the function returns
            std::size_t stack_base = 0;
            std::size_t args_base = 0;
            // How the result of a native function is given
            FrameKind deliver = FrameKind::Call;
        };

        // The main memory
        Memory& memory;

        // The lists of instructions
        std::vector<std::vector<Instruction>> lists;

        // The index of the current list of instructions
        std::size_t list_index = 0;

        // The index of the next instruction in the current list
        std::size_t line = 0;

        // The global namespace
        Variable global;

        // The stack of operations
        std::vector<Variable> stack;

        // The arguments being prepared for calls
        std::vector<std::vector<Variable>> pending_args;

        // The frames (the first one is the global scope)
        std::vector<Frame> frames;

        // A "try" block being executed
        struct Handler {
            // The position of the "catch" block
            std::size_t list = 0;
            std::size_t line = 0;
            // The sizes to restore when an error is caught
            std::size_t depth = 0;
            std::size_t stack_size = 0;
            std::size_t args_size = 0;
        };

        // The "try" blocks being executed
        std::vector<Handler> handlers;

        // The first handler usable by each call made by the application (errors can't cross native functions)
        std::vector<std::size_t> handler_floors;

        // The position of the instruction being executed (for the errors)
        SourcePosition executing;

        // The result of the last call made by the application
        Variable host_result;

        // The number of instructions being executed (several with the native functions)
        std::size_t running = 0;

        // Lists of instructions replaced during an execution (the instruction being executed
        // is used by reference, its list is kept until the end of the execution)
        std::vector<std::vector<Instruction>> retired;

        // The maximum number of frames
        std::size_t max_depth = 100000;

    public:

        /**
         * Constructor of an empty state
         *
         * @param memory The main memory
         */
        explicit State(Memory& memory);

        State(const State&) = delete;
        State& operator=(const State&) = delete;

        /**
         * Return the main memory
         */
        Memory& GetMemory() const noexcept { return this->memory; }

        ////// INSTRUCTIONS //////

        /**
         * Add a list of instructions, executed after the current ones
         *
         * @return The index of the list
         */
        std::size_t AddInstructions(std::vector<Instruction> instructions);

        /**
         * Add an instruction at the end of the last list
         */
        void AddInstruction(Instruction instruction);

        /**
         * Return the number of lists of instructions
         */
        std::size_t GetInstructionListCount() const noexcept { return this->lists.size(); }

        /**
         * Return a list of instructions
         */
        const std::vector<Instruction>& GetInstructions(std::size_t index) const { return this->lists.at(index); }

        /**
         * Return the index of the current list of instructions
         */
        std::size_t GetListIndex() const noexcept { return this->list_index; }

        /**
         * Return the index of the next instruction in the current list
         */
        std::size_t GetLine() const noexcept { return this->line; }

        ////// EXECUTION //////

        /**
         * Execute every remaining instruction
         *
         * @return The value on the top of the stack (undefined if the stack is empty)
         */
        Variable Execute();

        /**
         * Execute the next instruction
         *
         * @return The value on the top of the stack (undefined if the stack is empty)
         */
        Variable ExecuteSingle();

        /**
         * Execute an instruction that isn't stored in the lists (jumps are relative to the current list)
         *
         * @return The value on the top of the stack (undefined if the stack is empty)
         */
        Variable ExecuteInstruction(const Instruction& instruction);

        /**
         * Indicate if there is no instruction to execute
         */
        bool Finished() const noexcept;

        /**
         * Call a function (a callback or an instance with an overloaded call operator)
         * and execute it until it returns
         *
         * @param callee The function
         * @param args The arguments
         * @return The returned value
         */
        Variable Call(const Variable& callee, std::vector<Variable> args = {});

        /**
         * Change the maximum number of nested calls (deeper calls return undefined)
         */
        void SetMaxCallDepth(std::size_t depth) noexcept { this->max_depth = depth; }

        ////// VARIABLES //////

        /**
         * Find a variable visible from the current position
         *
         * @return The variable or an empty variable if not found
         */
        Variable GetVariable(std::string_view name) const;

        /**
         * Declare a variable in the global namespace
         */
        void DeclareVariable(std::string_view name, Variable value);

        /**
         * Return the global namespace
         */
        const Variable& GetGlobalNamespace() const noexcept { return this->global; }

        /**
         * Select the current namespace ("global" or names separated by dots, created if needed)
         */
        void UseNamespace(std::string_view path = "global");

        /**
         * Return the value of "this" in the current function (undefined if not set)
         */
        Variable GetThis() const;

        /**
         * Return the number of arguments of the current function
         */
        std::size_t GetArgsCount() const;

        /**
         * Return an argument of the current function (undefined if it doesn't exist)
         */
        Variable GetArg(std::size_t index) const;

        /**
         * Return the stack of operations
         */
        const std::vector<Variable>& GetStack() const noexcept { return this->stack; }

        /**
         * Return the number of nested frames (calls and scopes)
         */
        std::size_t GetDepth() const noexcept { return this->frames.size(); }

        ////// PERSISTENCE //////

        /**
         * Save the state (instructions, position, stacks and every object reachable from them)
         *
         * @throw SerializationError if an object can't be saved (unregistered native function)
         */
        void Save(std::ostream& stream) const;

        /**
         * Replace the content of the state by a saved state
         * The native functions must be registered in the memory before.
         *
         * @throw SerializationError if the stream is invalid
         */
        void Load(std::istream& stream);

    private:

        /**
         * Execute the next instruction
         *
         * @return false if there is no instruction to execute
         */
        bool Step();

        /**
         * Execute an instruction, handle the errors with the "try" blocks
         *
         * @throw ScriptError if the error isn't caught
         */
        void Run(const Instruction& instr);

        /**
         * Execute an instruction
         */
        void Dispatch(const Instruction& instr);

        /**
         * Raise an error of the engine
         */
        [[noreturn]] void Raise(const std::string& message);

        /**
         * Give an error to the last "try" block
         *
         * @return false if there is no usable "try" block
         */
        bool Catch(const ScriptError& error);

        /**
         * Return the first usable handler
         */
        std::size_t HandlerFloor() const noexcept { return this->handler_floors.empty() ? 0 : this->handler_floors.back(); }

        /**
         * Return the top of the stack or an undefined object
         */
        Variable Top() const;

        /**
         * Remove and return the top of the stack
         */
        Variable Pop();

        /**
         * Return the frame of the current function (the first frame that isn't a scope)
         */
        const Frame& FunctionFrame() const;

        /**
         * Call a function
         * Natives are executed immediately, script functions push a frame and their result is
         * delivered when they return.
         *
         * @param callee The function
         * @param args The arguments
         * @param kind The kind of call
         * @param self The value of "this" (if empty, the one bound to the callback)
         */
        void Invoke(const Variable& callee, std::vector<Variable> args, FrameKind kind, const Variable& self = Variable());

        /**
         * Give the result of a call
         */
        void Deliver(FrameKind kind, Variable result);

        /**
         * Return from the current function
         */
        void Return();

        /**
         * Apply an operator on the values of the stack
         */
        void Operation(Operator op);

        /**
         * Create an instance of a class and call its constructor
         */
        void Instantiate(const Variable& base, std::vector<Variable> args);

    };

}

#endif //LITESCRIPT_STATE_HPP
