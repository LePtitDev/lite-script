#ifndef LITESCRIPT_OPERATIONS_HPP
#define LITESCRIPT_OPERATIONS_HPP

#include <compare>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "object.hpp"

namespace LiteScript {

    // A call to an overloaded operator of a class that must be executed by a state
    struct OverloadCall {

        // The callback of the operator
        Variable callback;

        // The instance (value of "this")
        Variable self;

        // The arguments
        std::vector<Variable> args;

    };

    // The result of an operation: a variable or a call to execute
    using OperationResult = std::variant<Variable, OverloadCall>;

    /**
     * Indicate if a value is considered as true in a condition
     * (false for undefined, null, false, 0, NaN and empty strings)
     */
    bool IsTruthy(const Variable& v);

    /**
     * Return a string that describes the content of an object (strings are quoted)
     */
    std::string ToString(const Variable& v);

    /**
     * Return the text of a value: the content of strings and characters, the description of the other types
     */
    String ToText(const Variable& v);

    /**
     * Create a new object that contains a copy of a value (this is the semantic of the assignment):
     * arrays, instances, classes and namespaces are shared (the new object references them),
     * characters become strings, the other values are copied
     */
    Variable Copy(Memory& memory, const Variable& v);

    /**
     * Create a new independent value: a new array with copies of the elements, a new instance or
     * namespace with the same members (the other values are copied like Copy)
     */
    Variable Clone(Memory& memory, const Variable& v);

    /**
     * Convert a value to another type
     *
     * @return The converted value or a null object if the conversion isn't possible
     */
    Variable Convert(Memory& memory, const Variable& v, Type type);

    /**
     * Apply an operator (every operator except Operator::Call)
     * Unary operators ignore the second operand.
     * If the first operand is an instance of a class that overloads the operator, an OverloadCall is returned.
     *
     * @param memory The memory where results are created
     * @param op The operator
     * @param a The first operand (modified by assignment operators)
     * @param b The second operand
     */
    OperationResult Operate(Memory& memory, Operator op, const Variable& a, const Variable& b = Variable());

    /**
     * Return a member of a value ("a.name"), methods are bound to the value
     *
     * @return The member or an undefined object if it doesn't exist (assigning a value to this
     *         object creates the member in arrays, instances, classes and namespaces)
     */
    Variable GetMember(Memory& memory, const Variable& v, std::string_view name);

    /**
     * Indicate if an operator takes two operands
     */
    bool IsBinary(Operator op) noexcept;

    /**
     * Compare two values with the semantic of "==" (without overloaded operators)
     */
    bool ValuesEqual(const Variable& a, const Variable& b);

    /**
     * Compare two values with the semantic of "<" and ">" (without overloaded operators)
     *
     * @return unordered if the values can't be compared
     */
    std::partial_ordering CompareValues(const Variable& a, const Variable& b);

}

#endif //LITESCRIPT_OPERATIONS_HPP
