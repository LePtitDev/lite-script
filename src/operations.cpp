#include <litescript/operations.hpp>
#include <litescript/memory.hpp>

#include <algorithm>
#include <cmath>
#include <compare>
#include <optional>
#include <ostream>
#include <stdexcept>

namespace {

    using namespace LiteScript;

    // Maximum index of an array element created by an access
    constexpr std::int64_t MAX_ARRAY_INDEX = std::int64_t(1) << 24;

    // Return the number of a value that can be used as a number (booleans and numbers)
    std::optional<Number> ToNumber(const Variable& v) {
        if (v.Is<Number>())
            return v.As<Number>();
        if (v.Is<bool>())
            return Number(v.As<bool>() ? 1 : 0);
        return std::nullopt;
    }

    // Return the character referenced by a character object (an empty string if it's invalid)
    String CharacterText(const Character& c) {
        if (c.string.Is<String>() && c.index < c.string.As<String>().GetLength())
            return String(c.string.As<String>()[c.index]);
        return String();
    }

    // Return the string referenced by a character object (nullptr if the character is invalid)
    String * CharacterString(const Character& c) {
        if (c.string.Is<String>() && c.index < c.string.As<String>().GetLength())
            return &c.string.As<String>();
        return nullptr;
    }

    // Indicate if a type is shared by assignment
    bool IsReferenceType(Type type) {
        return type == Type::Array || type == Type::Class || type == Type::ClassObject || type == Type::Namespace;
    }

    // Copy the content of an object with the assignment semantic: arrays, instances, classes and
    // namespaces are shared (the result is a reference), the other values are copied
    Data CopyData(const Variable& v) {
        if (!v || v.Is<Undefined>())
            return Undefined{};
        if (v.Is<Character>())
            return CharacterText(v.As<Character>());
        if (IsReferenceType(v.GetType()))
            return Reference{ Variable(v->Target()) };
        return v->Target().data;
    }

    // Create an undefined object that becomes an element of the container when a value is assigned to it
    Variable CreateMissing(Memory& memory, const Variable& container, std::variant<std::size_t, std::string> key) {
        return memory.Create(Undefined{ Location{ container, std::move(key) } });
    }

    // Insert an object at the position where it was missing
    void Materialize(const Location& origin, const Variable& v) {
        if (const auto * index = std::get_if<std::size_t>(&origin.key)) {
            if (!origin.container.Is<Array>())
                return;
            auto& elements = origin.container.As<Array>().indexed;
            // The holes contain undefined values
            while (elements.size() < *index)
                elements.push_back(v->GetMemory().CreateUndefined());
            if (elements.size() == *index)
                elements.push_back(v);
            else
                elements[*index] = v;
            return;
        }
        const std::string& name = std::get<std::string>(origin.key);
        switch (origin.container.GetType()) {
            case Type::Array:
                origin.container.As<Array>().named.Set(name, v);
                break;
            case Type::ClassObject:
                origin.container.As<ClassObject>().members.Set(name, v);
                break;
            case Type::Class:
                origin.container.As<Class>().statics.Set(name, v);
                break;
            case Type::Namespace:
                origin.container.As<Namespace>().variables.Set(name, v);
                break;
            default:
                break;
        }
    }

    // Create a callback object bound to an instance
    Variable Bind(Memory& memory, const Variable& callback, const Variable& self) {
        Callback bound = callback.As<Callback>();
        bound.self = self;
        return memory.Create(std::move(bound));
    }

    // Repeat a string, return nullopt if the count isn't a number or the result is too long
    std::optional<String> Repeat(const String& s, const Variable& count) {
        auto n = ToNumber(count);
        if (!n)
            return std::nullopt;
        try {
            return s.Repeat(n->ToInteger());
        }
        catch (const std::length_error&) {
            return std::nullopt;
        }
    }

    // Equality comparison without overloading
    bool Equals(const Variable& a, const Variable& b) {
        switch (a.GetType()) {
            case Type::Undefined:
                return b.Is<Undefined>() || !b;
            case Type::Null:
                return b.Is<Null>();
            case Type::Boolean:
                return b.Is<bool>() && a.As<bool>() == b.As<bool>();
            case Type::Number:
                return b.Is<Number>() && a.As<Number>() == b.As<Number>();
            case Type::String:
                if (b.Is<Character>())
                    return a.As<String>() == CharacterText(b.As<Character>());
                return b.Is<String>() && a.As<String>() == b.As<String>();
            case Type::Character:
                return CharacterText(a.As<Character>()) == ToText(b);
            case Type::Callback:
                return b.Is<Callback>() && a.As<Callback>().SameTarget(b.As<Callback>());
            default:
                return b && &a->Target() == &b->Target();
        }
    }

    // Ordering comparison without overloading
    std::partial_ordering Compare(const Variable& a, const Variable& b) {
        switch (a.GetType()) {
            case Type::Number: {
                auto n = ToNumber(b);
                if (!n)
                    return std::partial_ordering::unordered;
                const Number& x = a.As<Number>();
                if (x < *n)
                    return std::partial_ordering::less;
                if (x > *n)
                    return std::partial_ordering::greater;
                if (x == *n)
                    return std::partial_ordering::equivalent;
                return std::partial_ordering::unordered;
            }
            case Type::String:
                return a.As<String>() <=> ToText(b);
            case Type::Character:
                return CharacterText(a.As<Character>()) <=> ToText(b);
            default:
                return std::partial_ordering::unordered;
        }
    }

    // Apply an arithmetic operation between numbers
    template <typename F>
    Variable NumberOperation(Memory& memory, const Variable& a, const Variable& b, F&& f) {
        auto n = ToNumber(b);
        if (!a.Is<Number>() || !n)
            return memory.CreateNull();
        return memory.Create(f(a.As<Number>(), *n));
    }

    // Apply an arithmetic operation between numbers and store the result in the first operand
    template <typename F>
    Variable NumberAssignOperation(const Variable& a, const Variable& b, F&& f) {
        auto n = ToNumber(b);
        if (n)
            a.As<Number>() = f(a.As<Number>(), *n);
        else
            a->data = Null{};
        return a;
    }

    // Return the element of an array (an undefined object that is created by an assignment if it doesn't exist)
    Variable ArrayGet(Memory& memory, const Variable& a, const Variable& key) {
        Array& array = a.As<Array>();
        if (key.Is<Number>()) {
            double index = key.As<Number>().ToFloat();
            if (!(index >= 0) || index >= static_cast<double>(MAX_ARRAY_INDEX) || std::trunc(index) != index)
                return memory.CreateUndefined();
            auto i = static_cast<std::size_t>(index);
            if (i < array.indexed.size())
                return array.indexed[i];
            return CreateMissing(memory, Variable(a->Target()), i);
        }
        if (key.Is<String>() || key.Is<Character>()) {
            std::string name = ToText(key).ToUTF8();
            if (Variable v = array.named.Find(name))
                return v;
            return CreateMissing(memory, Variable(a->Target()), std::move(name));
        }
        return memory.CreateUndefined();
    }

    // Description of a value, the visited containers are ignored to avoid infinite recursions
    void Describe(const Variable& v, std::string& out, std::vector<const Object *>& visiting) {
        auto enter = [&]() {
            const Object * target = &v->Target();
            if (std::find(visiting.begin(), visiting.end(), target) != visiting.end()) {
                out += "{...}";
                return false;
            }
            visiting.push_back(target);
            return true;
        };
        auto describe_members = [&](const MemberList& list, bool skip_undefined) {
            bool first = true;
            for (const auto& [name, value] : list) {
                if (skip_undefined && value.Is<Undefined>())
                    continue;
                if (!first)
                    out += ',';
                first = false;
                out += name;
                out += ':';
                Describe(value, out, visiting);
            }
        };
        switch (v.GetType()) {
            case Type::Undefined:
                out += "undefined";
                break;
            case Type::Null:
                out += "null";
                break;
            case Type::Boolean:
                out += v.As<bool>() ? "true" : "false";
                break;
            case Type::Number:
                out += v.As<Number>().ToString();
                break;
            case Type::String:
                out += '"';
                out += v.As<String>().ToUTF8();
                out += '"';
                break;
            case Type::Character:
                out += CharacterText(v.As<Character>()).ToUTF8();
                break;
            case Type::Callback:
                out += "function";
                break;
            case Type::Array: {
                if (!enter())
                    break;
                const Array& array = v.As<Array>();
                out += '{';
                bool first = true;
                for (std::size_t i = 0, sz = array.indexed.size(); i < sz; i++) {
                    if (array.indexed[i].Is<Undefined>())
                        continue;
                    if (!first)
                        out += ',';
                    first = false;
                    out += '[' + std::to_string(i) + "]:";
                    Describe(array.indexed[i], out, visiting);
                }
                for (const auto& [name, value] : array.named) {
                    if (value.Is<Undefined>())
                        continue;
                    if (!first)
                        out += ',';
                    first = false;
                    out += "['" + name + "']:";
                    Describe(value, out, visiting);
                }
                out += '}';
                visiting.pop_back();
                break;
            }
            case Type::Class:
                out += "class";
                break;
            case Type::ClassObject:
                if (!enter())
                    break;
                out += '{';
                describe_members(v.As<ClassObject>().members, false);
                out += '}';
                visiting.pop_back();
                break;
            case Type::Namespace:
                if (!enter())
                    break;
                out += '{';
                describe_members(v.As<Namespace>().variables, false);
                out += '}';
                visiting.pop_back();
                break;
        }
    }

}

bool LiteScript::IsTruthy(const Variable& v) {
    switch (v.GetType()) {
        case Type::Undefined:
        case Type::Null:
            return false;
        case Type::Boolean:
            return v.As<bool>();
        case Type::Number: {
            double d = v.As<Number>().ToFloat();
            return d != 0 && !std::isnan(d);
        }
        case Type::String:
            return v.As<String>().GetLength() > 0;
        default:
            return true;
    }
}

std::string LiteScript::ToString(const Variable& v) {
    std::string out;
    std::vector<const Object *> visiting;
    Describe(v, out, visiting);
    return out;
}

LiteScript::String LiteScript::ToText(const Variable& v) {
    if (v.Is<String>())
        return v.As<String>();
    if (v.Is<Character>())
        return CharacterText(v.As<Character>());
    return String(ToString(v));
}

LiteScript::Variable LiteScript::Copy(Memory& memory, const Variable& v) {
    return memory.Create(CopyData(v));
}

LiteScript::Variable LiteScript::Convert(Memory& memory, const Variable& v, Type type) {
    switch (type) {
        case Type::Boolean:
            return memory.CreateBoolean(IsTruthy(v));
        case Type::Number:
            if (auto n = ToNumber(v))
                return memory.CreateNumber(*n);
            return memory.CreateNull();
        case Type::String:
            return memory.Create(ToText(v));
        default:
            if (v.GetType() == type)
                return Copy(memory, v);
            return memory.CreateNull();
    }
}

bool LiteScript::IsBinary(Operator op) noexcept {
    switch (op) {
        case Operator::UnaryPlus:
        case Operator::UnaryMinus:
        case Operator::PreIncrement:
        case Operator::PostIncrement:
        case Operator::PreDecrement:
        case Operator::PostDecrement:
        case Operator::LogicalNot:
        case Operator::BitwiseNot:
            return false;
        default:
            return true;
    }
}

LiteScript::OperationResult LiteScript::Operate(Memory& memory, Operator op, const Variable& a, const Variable& b) {
    // Overloaded operators of class instances
    if (a.Is<ClassObject>() && op != Operator::Call) {
        if (const Class * c = a.As<ClassObject>().GetClass()) {
            if (const Variable& callback = c->GetOperator(op); callback.Is<Callback>()) {
                OverloadCall call{ callback, a, {} };
                if (IsBinary(op))
                    call.args.push_back(b ? b : memory.CreateUndefined());
                return call;
            }
        }
    }

    switch (op) {
        case Operator::Assign: {
            // The objects that contain arrays, instances, classes or namespaces can't be replaced
            // (variables reference them, only the variables are assigned)
            if (!a->IsReference() && IsReferenceType(a->GetType()))
                return a;
            if (a.Is<Character>()) {
                const Character& c = a.As<Character>();
                if (String * s = CharacterString(c))
                    s->Replace(c.index, 1, ToText(b));
                return a;
            }
            Data value = CopyData(b);
            // An object can't reference itself
            if (const auto * r = std::get_if<Reference>(&value); r != nullptr && r->target.Get() == a.Get())
                return a;
            Location origin;
            if (auto * u = std::get_if<Undefined>(&a->data))
                origin = std::move(u->origin);
            a->data = std::move(value);
            // A missing element of a container is created by the assignment
            if (origin.container)
                Materialize(origin, a);
            return a;
        }

        case Operator::UnaryPlus:
            if (a.Is<Number>())
                return memory.CreateNumber(a.As<Number>());
            return memory.CreateNull();

        case Operator::UnaryMinus:
            if (a.Is<Number>())
                return memory.CreateNumber(-a.As<Number>());
            return memory.CreateNull();

        case Operator::PreIncrement:
            if (a.Is<Number>())
                ++a.As<Number>();
            return a;

        case Operator::PreDecrement:
            if (a.Is<Number>())
                --a.As<Number>();
            return a;

        case Operator::PostIncrement:
            if (a.Is<Number>())
                return memory.CreateNumber(a.As<Number>()++);
            return memory.CreateNull();

        case Operator::PostDecrement:
            if (a.Is<Number>())
                return memory.CreateNumber(a.As<Number>()--);
            return memory.CreateNull();

        case Operator::Add:
            switch (a.GetType()) {
                case Type::Number:
                    return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x + y; });
                case Type::String:
                    return memory.Create(a.As<String>() + ToText(b));
                case Type::Character:
                    return memory.Create(CharacterText(a.As<Character>()) + ToText(b));
                case Type::Array: {
                    if (!b.Is<Array>())
                        return memory.CreateNull();
                    // A new array with copies of the elements of both arrays
                    Variable result = Clone(memory, a);
                    Array& merged = result.As<Array>();
                    for (const Variable& value : b.As<Array>().indexed)
                        merged.indexed.push_back(Copy(memory, value));
                    for (const auto& [name, value] : b.As<Array>().named)
                        merged.named.Set(name, Copy(memory, value));
                    return result;
                }
                default:
                    return memory.CreateNull();
            }

        case Operator::Subtract:
            return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x - y; });

        case Operator::Multiply:
            if (a.Is<String>() || a.Is<Character>()) {
                if (auto s = Repeat(ToText(a), b))
                    return memory.Create(std::move(*s));
                return memory.CreateNull();
            }
            return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x * y; });

        case Operator::Divide:
            return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x / y; });

        case Operator::Modulo:
            return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x % y; });

        case Operator::Equal:
            return memory.CreateBoolean(Equals(a, b));

        case Operator::NotEqual:
            return memory.CreateBoolean(!Equals(a, b));

        case Operator::Greater:
            return memory.CreateBoolean(std::is_gt(Compare(a, b)));

        case Operator::Less:
            return memory.CreateBoolean(std::is_lt(Compare(a, b)));

        case Operator::GreaterOrEqual:
            return memory.CreateBoolean(std::is_gteq(Compare(a, b)));

        case Operator::LessOrEqual:
            return memory.CreateBoolean(std::is_lteq(Compare(a, b)));

        case Operator::LogicalNot:
            return memory.CreateBoolean(!IsTruthy(a));

        case Operator::LogicalAnd:
            return memory.CreateBoolean(IsTruthy(a) && IsTruthy(b));

        case Operator::LogicalOr:
            return memory.CreateBoolean(IsTruthy(a) || IsTruthy(b));

        case Operator::BitwiseNot:
            if (a.Is<Number>())
                return memory.CreateNumber(~a.As<Number>());
            return memory.CreateNull();

        case Operator::BitwiseAnd:
            return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x & y; });

        case Operator::BitwiseOr:
            // "a | b" gives b when a is undefined or null and a for the other non numeric types
            if (a.Is<Number>())
                return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x | y; });
            if (a.Is<Undefined>() || a.Is<Null>())
                return b ? b : memory.CreateUndefined();
            return a;

        case Operator::BitwiseXor:
            return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x ^ y; });

        case Operator::LeftShift:
            return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x << y; });

        case Operator::RightShift:
            return NumberOperation(memory, a, b, [](const Number& x, const Number& y) { return x >> y; });

        case Operator::AddAssign:
            switch (a.GetType()) {
                case Type::Number:
                    return NumberAssignOperation(a, b, [](const Number& x, const Number& y) { return x + y; });
                case Type::String: {
                    String text = ToText(b);
                    a.As<String>() += text;
                    return a;
                }
                case Type::Character: {
                    // The text is inserted after the character
                    const Character& c = a.As<Character>();
                    String text = ToText(b);
                    if (String * s = CharacterString(c))
                        s->Replace(c.index + 1, 0, text);
                    return a;
                }
                default:
                    return a;
            }

        case Operator::SubtractAssign:
            if (a.Is<Number>())
                return NumberAssignOperation(a, b, [](const Number& x, const Number& y) { return x - y; });
            return a;

        case Operator::MultiplyAssign:
            if (a.Is<Number>())
                return NumberAssignOperation(a, b, [](const Number& x, const Number& y) { return x * y; });
            if (a.Is<String>()) {
                if (auto s = Repeat(a.As<String>(), b))
                    a.As<String>() = std::move(*s);
                else
                    a->data = Null{};
                return a;
            }
            if (a.Is<Character>()) {
                const Character& c = a.As<Character>();
                String * s = CharacterString(c);
                auto repeated = Repeat(CharacterText(c), b);
                if (s != nullptr && repeated)
                    s->Replace(c.index, 1, *repeated);
                return a;
            }
            return a;

        case Operator::DivideAssign:
            if (a.Is<Number>())
                return NumberAssignOperation(a, b, [](const Number& x, const Number& y) { return x / y; });
            return a;

        case Operator::Array:
            if (a.Is<Array>())
                return ArrayGet(memory, a, b);
            if (a.Is<String>() && b.Is<Number>()) {
                double index = b.As<Number>().ToFloat();
                if (index >= 0 && std::trunc(index) == index && index < static_cast<double>(a.As<String>().GetLength()))
                    return memory.Create(Character{ a, static_cast<std::size_t>(index) });
            }
            return memory.CreateUndefined();

        case Operator::Call:
            break;
    }
    return memory.CreateUndefined();
}

LiteScript::Variable LiteScript::GetMember(Memory& memory, const Variable& v, std::string_view name) {
    // The members that don't exist are created when a value is assigned to them
    Variable container = v ? Variable(v->Target()) : Variable();
    switch (v.GetType()) {
        case Type::Array: {
            if (name == "length")
                return memory.CreateNumber(static_cast<std::int64_t>(v.As<Array>().indexed.size()));
            if (Variable member = v.As<Array>().named.Find(name))
                return member.Is<Callback>() ? Bind(memory, member, container) : member;
            if (Variable method = memory.FindMethod(Type::Array, name))
                return Bind(memory, method, container);
            return CreateMissing(memory, container, std::string(name));
        }
        case Type::String:
            if (name == "length")
                return memory.CreateNumber(static_cast<std::int64_t>(v.As<String>().GetLength()));
            break;
        case Type::ClassObject: {
            const ClassObject& instance = v.As<ClassObject>();
            if (Variable member = instance.members.Find(name))
                return member;
            if (const Class * c = instance.GetClass()) {
                if (Variable member = c->members.Find(name); member.Is<Callback>())
                    return Bind(memory, member, container);
            }
            return CreateMissing(memory, container, std::string(name));
        }
        case Type::Class:
            if (Variable member = v.As<Class>().statics.Find(name))
                return member;
            return CreateMissing(memory, container, std::string(name));
        case Type::Namespace:
            if (Variable member = v.As<Namespace>().variables.Find(name))
                return member;
            return CreateMissing(memory, container, std::string(name));
        default:
            break;
    }
    // The methods defined by the application for the type
    if (v) {
        if (Variable method = memory.FindMethod(v.GetType(), name))
            return Bind(memory, method, container);
    }
    return memory.CreateUndefined();
}

bool LiteScript::ValuesEqual(const Variable& a, const Variable& b) {
    return Equals(a, b);
}

std::partial_ordering LiteScript::CompareValues(const Variable& a, const Variable& b) {
    return Compare(a, b);
}

LiteScript::Variable LiteScript::Clone(Memory& memory, const Variable& v) {
    if (v.Is<Array>()) {
        const Array& source = v.As<Array>();
        Array array;
        array.indexed.reserve(source.indexed.size());
        for (const Variable& e : source.indexed)
            array.indexed.push_back(Copy(memory, e));
        for (const auto& [name, value] : source.named)
            array.named.Set(name, Copy(memory, value));
        return memory.Create(std::move(array));
    }
    if (v.Is<Namespace>() || v.Is<ClassObject>() || v.Is<Class>())
        return memory.Create(Data(v->Target().data));
    return Copy(memory, v);
}

std::string LiteScript::Variable::ToString() const {
    return LiteScript::ToString(*this);
}

std::ostream& LiteScript::operator<<(std::ostream& stream, const Variable& variable) {
    return stream << LiteScript::ToString(variable);
}
