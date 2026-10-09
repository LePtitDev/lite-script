#ifndef LITESCRIPT_VARIABLE_HPP
#define LITESCRIPT_VARIABLE_HPP

#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

namespace LiteScript {

    // Class declarations
    class Object;

    // Types of the objects (same order as the alternatives of LiteScript::Data)
    enum class Type : std::uint8_t {
        Undefined,
        Null,
        Boolean,
        Number,
        String,
        Character,
        Callback,
        Array,
        Class,
        ClassObject,
        Namespace
    };

    // Number of types
    constexpr std::size_t TYPE_COUNT = static_cast<std::size_t>(Type::Namespace) + 1;

    /**
     * Return the name of a type ("undefined", "null", "boolean"...)
     */
    std::string_view GetTypeName(Type type) noexcept;

    // A counted reference to an object of a memory
    // Copying a variable shares the object, the object is destroyed when
    // its last reference disappears (cycles are destroyed by Memory::CollectGarbage)
    class Variable {

        // The referenced object (nullptr for an empty variable)
        Object * object = nullptr;

    public:

        /**
         * Empty variable constructor
         */
        Variable() noexcept = default;

        /**
         * Create a new reference to an object
         *
         * @param obj The object
         */
        explicit Variable(Object& obj) noexcept;

        Variable(const Variable& v) noexcept;
        Variable(Variable&& v) noexcept : object(v.object) { v.object = nullptr; }
        Variable& operator=(const Variable& v) noexcept;
        Variable& operator=(Variable&& v) noexcept;
        ~Variable();

        /**
         * Release the reference
         */
        void Reset() noexcept;

        /**
         * Indicate if the variable references an object
         */
        explicit operator bool() const noexcept { return this->object != nullptr; }

        /**
         * Return the referenced object (nullptr if empty)
         */
        Object * Get() const noexcept { return this->object; }

        Object& operator*() const noexcept { return *this->object; }
        Object * operator->() const noexcept { return this->object; }

        /**
         * Return the type of the referenced object (undefined if empty)
         */
        Type GetType() const noexcept;

        /**
         * Indicate if the referenced object contains a data of type T
         */
        template <typename T>
        bool Is() const noexcept;

        /**
         * Return the data of the referenced object (the type must match)
         */
        template <typename T>
        T& As() const;

        /**
         * Return a string that describes the content of the object
         */
        std::string ToString() const;

        /**
         * Indicate if two variables reference the same object
         */
        friend bool operator==(const Variable& a, const Variable& b) noexcept { return a.object == b.object; }

    };

    /**
     * Send the description of the object in the stream
     */
    std::ostream& operator<<(std::ostream& stream, const Variable& variable);

}

#endif //LITESCRIPT_VARIABLE_HPP
