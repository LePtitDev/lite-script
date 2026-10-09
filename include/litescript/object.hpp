#ifndef LITESCRIPT_OBJECT_HPP
#define LITESCRIPT_OBJECT_HPP

#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "number.hpp"
#include "string.hpp"
#include "variable.hpp"

namespace LiteScript {

    // Class declarations
    class Memory;
    class State;

    // Operators that a class can overload (the order is used by the "class.push-operator" instruction)
    enum class Operator : std::uint8_t {
        Assign,
        UnaryPlus,
        UnaryMinus,
        PreIncrement,
        PostIncrement,
        PreDecrement,
        PostDecrement,
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,
        Equal,
        NotEqual,
        Greater,
        Less,
        GreaterOrEqual,
        LessOrEqual,
        LogicalNot,
        LogicalAnd,
        LogicalOr,
        BitwiseNot,
        BitwiseAnd,
        BitwiseOr,
        BitwiseXor,
        LeftShift,
        RightShift,
        AddAssign,
        SubtractAssign,
        MultiplyAssign,
        DivideAssign,
        Array,
        Call
    };

    // Number of operators
    constexpr std::size_t OPERATOR_COUNT = static_cast<std::size_t>(Operator::Call) + 1;

    // An ordered list of named variables
    class MemberList {

        // The members
        std::vector<std::pair<std::string, Variable>> items;

    public:

        std::size_t Count() const noexcept { return this->items.size(); }
        bool Empty() const noexcept { return this->items.empty(); }
        const std::string& GetName(std::size_t index) const { return this->items[index].first; }
        const Variable& GetValue(std::size_t index) const { return this->items[index].second; }

        /**
         * Return the index of a member (or nullopt if not found)
         */
        std::optional<std::size_t> IndexOf(std::string_view name) const noexcept;

        /**
         * Return the variable of a member (or an empty variable if not found)
         */
        Variable Find(std::string_view name) const;

        /**
         * Define a member, replace the previous one with the same name
         */
        void Set(std::string_view name, Variable value);

        auto begin() const noexcept { return this->items.begin(); }
        auto end() const noexcept { return this->items.end(); }

    };

    // The position of an element that doesn't exist yet in a container
    // (an index of an array, or a name of an array, an instance, a class or a namespace)
    struct Location {

        // The container (empty if the value isn't an element of a container)
        Variable container;

        // The index or the name of the element
        std::variant<std::size_t, std::string> key;

    };

    // Value of the type "undefined"
    struct Undefined {

        // The element that is created if a value is assigned to this object ("a[5] = x" when a[5] doesn't exist)
        Location origin;

    };

    // Value of the type "null"
    struct Null {};

    // A character of a string object (the result of "string[index]"),
    // assigning it modifies the string
    struct Character {

        // The string object
        Variable string;

        // The index of the character in the string
        std::size_t index = 0;

    };

    // Namespaces resolution of a script context
    class Namer {

    public:

        // The global namespace
        Variable global;

        // The namespace selected by the "namespace" instruction
        Variable current;

        // The pushed scopes (namespaces of local variables or classes)
        std::vector<Variable> scopes;

        Namer() = default;

        /**
         * Constructor of a namer that uses the global namespace
         */
        explicit Namer(Variable global_namespace) : global(global_namespace), current(std::move(global_namespace)) {}

        /**
         * Find a variable in scopes, then in the current namespace and then in the global namespace
         *
         * @return The variable or an empty variable if not found
         */
        Variable Find(std::string_view name) const;

        /**
         * Declare a variable in the last namespace scope (or the current namespace)
         */
        void Declare(std::string_view name, Variable value);

        /**
         * Push a scope (a namespace or a class)
         */
        void Push(Variable scope);

        /**
         * Pop the last scope if it's the indicated one
         */
        void Pop(const Variable& scope);

    };

    // A function implemented by the application
    struct NativeFunction {

        // Signature of the functions
        using Signature = std::function<Variable(State& state, std::vector<Variable>& args)>;

        // The name used to save and load the function (empty if not registered)
        std::string name;

        // The function
        Signature function;

    };

    // A function of the script
    // (declared outside of Callback: GCC can't default-construct a std::variant of a nested
    // class that has default member initializers while the enclosing class is incomplete)
    struct ScriptFunction {

        // The index of the instructions list
        std::size_t list = 0;

        // The index of the first instruction
        std::size_t line = 0;

        // The namespaces visible by the function
        Namer scope;

    };

    // Content of a function
    class Callback {

    public:

        using ScriptFunction = LiteScript::ScriptFunction;

        // The function
        std::variant<ScriptFunction, std::shared_ptr<const NativeFunction>> target;

        // The value of "this" (empty if not bound)
        Variable self;

        bool IsNative() const noexcept { return this->target.index() == 1; }

        /**
         * Indicate if two callbacks call the same function
         */
        bool SameTarget(const Callback& c) const noexcept;

    };

    // Content of an associative array
    class Array {

    public:

        // The elements indexed by numbers
        std::vector<Variable> indexed;

        // The elements indexed by names
        MemberList named;

    };

    // Content of a class
    class Class {

    public:

        // The inherited classes
        std::vector<Variable> inherits;

        // The static members
        MemberList statics;

        // The members of the instances (callbacks are methods)
        MemberList members;

        // The overloaded operators (empty if not overloaded)
        std::array<Variable, OPERATOR_COUNT> operators;

        // The name of the constructor in members (empty if there is no constructor)
        std::string constructor;

        /**
         * Inherit the members and operators of a class
         *
         * @param parent The class object to inherit
         */
        void Inherit(const Variable& parent);

        /**
         * Define the constructor by its name
         *
         * @return false if the member doesn't exist or isn't a callback
         */
        bool DefineConstructor(std::string_view name);

        /**
         * Return the constructor (empty if there is no constructor)
         */
        Variable GetConstructor() const;

        /**
         * Return the overloaded operator (empty if not defined)
         */
        const Variable& GetOperator(Operator op) const noexcept { return this->operators[static_cast<std::size_t>(op)]; }

    };

    // Content of an instance of class
    class ClassObject {

    public:

        // The class of the instance
        Variable base;

        // The attributes
        MemberList members;

        /**
         * Return the class of the instance (nullptr if not a class anymore)
         */
        const Class * GetClass() const noexcept;

    };

    // Content of a namespace
    class Namespace {

    public:

        // The variables
        MemberList variables;

    };

    // A reference to another object: arrays, instances, classes and namespaces are shared by assignment,
    // the variables contain a reference to them (the references are followed by Variable::Is, As and GetType)
    struct Reference {

        // The referenced object
        Variable target;

    };

    // The content of an object (the index of the alternative is the LiteScript::Type, except for Reference)
    using Data = std::variant<Undefined, Null, bool, Number, String, Character, Callback, Array, Class, ClassObject, Namespace, Reference>;

    // An object stored in a memory
    class Object {

        friend class Memory;
        friend class Variable;

        // The memory that contains the object (nullptr if the memory has been destroyed)
        Memory * memory;

        // The number of variables that reference the object
        std::size_t references = 0;

        // The position of the object in the memory
        std::size_t slot = 0;

        // Garbage collector counter of external references
        std::size_t gc_references = 0;

        // Garbage collector mark
        bool gc_reachable = false;

        Object(Memory& mem, Data content) : memory(&mem), data(std::move(content)) {}

    public:

        // The memory blocks of the objects are reused (a cache per thread)
        static void * operator new(std::size_t size);
        static void operator delete(void * block, std::size_t size) noexcept;

        /**
         * Free the memory blocks kept by the current thread to create the next objects quickly
         */
        static void ReleaseCachedBlocks() noexcept;

        // The content of the object
        Data data;

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        /**
         * Return the object that contains the value (the referenced object for a reference, this object otherwise)
         */
        Object& Target() noexcept;
        const Object& Target() const noexcept;

        /**
         * Indicate if the object contains a reference to another object
         */
        bool IsReference() const noexcept { return std::holds_alternative<Reference>(this->data); }

        /**
         * Return the type of the value (the type of the referenced object for a reference)
         */
        Type GetType() const noexcept { return static_cast<Type>(this->Target().data.index()); }

        /**
         * Return the memory that contains the object
         */
        Memory& GetMemory() const noexcept { return *this->memory; }

        /**
         * Return the number of variables that reference the object
         */
        std::size_t GetReferenceCount() const noexcept { return this->references; }

    private:

        /**
         * Called when the last reference disappears
         */
        void Release() noexcept;

    };

    ////// OBJECT INLINE METHODS //////

    inline Object& Object::Target() noexcept {
        Object * obj = this;
        while (const Reference * r = std::get_if<Reference>(&obj->data)) {
            if (!r->target)
                break;
            obj = r->target.Get();
        }
        return *obj;
    }

    inline const Object& Object::Target() const noexcept {
        return const_cast<Object *>(this)->Target();
    }

    ////// VARIABLE INLINE METHODS //////

    inline Variable::Variable(Object& obj) noexcept : object(&obj) {
        ++obj.references;
    }

    inline Variable::Variable(const Variable& v) noexcept : object(v.object) {
        if (this->object != nullptr)
            ++this->object->references;
    }

    inline Variable& Variable::operator=(const Variable& v) noexcept {
        Variable tmp(v);
        std::swap(this->object, tmp.object);
        return *this;
    }

    inline Variable& Variable::operator=(Variable&& v) noexcept {
        Variable tmp(std::move(v));
        std::swap(this->object, tmp.object);
        return *this;
    }

    inline Variable::~Variable() {
        this->Reset();
    }

    inline void Variable::Reset() noexcept {
        Object * obj = std::exchange(this->object, nullptr);
        if (obj != nullptr && --obj->references == 0)
            obj->Release();
    }

    inline Type Variable::GetType() const noexcept {
        return this->object != nullptr ? this->object->GetType() : Type::Undefined;
    }

    template <typename T>
    bool Variable::Is() const noexcept {
        return this->object != nullptr && std::holds_alternative<T>(this->object->Target().data);
    }

    template <typename T>
    T& Variable::As() const {
        return std::get<T>(this->object->Target().data);
    }

}

#endif //LITESCRIPT_OBJECT_HPP
