#ifndef LITESCRIPT_PLAYGROUND_TOOLS_JSONVALUE_HPP
#define LITESCRIPT_PLAYGROUND_TOOLS_JSONVALUE_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// The messages of the language server and of the debug adapter
namespace Tools {

    // A JSON value (the members of an object keep their order)
    class Json {
    public:

        enum class Type { Null, Boolean, Number, String, Array, Object };

    private:

        Type type = Type::Null;
        bool boolean = false;
        double number = 0;
        std::string text;
        std::vector<Json> items;
        std::vector<std::string> keys;

    public:

        Json() = default;
        Json(std::nullptr_t) {}
        Json(bool value) : type(Type::Boolean), boolean(value) {}
        Json(int value) : type(Type::Number), number(value) {}
        Json(std::int64_t value) : type(Type::Number), number(static_cast<double>(value)) {}
        Json(std::size_t value) : type(Type::Number), number(static_cast<double>(value)) {}
        Json(double value) : type(Type::Number), number(value) {}
        Json(const char * value) : type(Type::String), text(value) {}
        Json(std::string value) : type(Type::String), text(std::move(value)) {}
        Json(std::string_view value) : type(Type::String), text(value) {}

        static Json MakeArray() {
            Json res;
            res.type = Type::Array;
            return res;
        }

        static Json MakeObject() {
            Json res;
            res.type = Type::Object;
            return res;
        }

        Type GetType() const noexcept { return this->type; }
        bool IsNull() const noexcept { return this->type == Type::Null; }
        bool IsString() const noexcept { return this->type == Type::String; }
        bool IsNumber() const noexcept { return this->type == Type::Number; }
        bool IsArray() const noexcept { return this->type == Type::Array; }
        bool IsObject() const noexcept { return this->type == Type::Object; }

        bool GetBool(bool default_value = false) const noexcept {
            return this->type == Type::Boolean ? this->boolean : default_value;
        }

        double GetNumber(double default_value = 0) const noexcept {
            return this->type == Type::Number ? this->number : default_value;
        }

        std::int64_t GetInt(std::int64_t default_value = 0) const noexcept {
            return this->type == Type::Number ? static_cast<std::int64_t>(this->number) : default_value;
        }

        const std::string& GetString() const noexcept {
            static const std::string empty;
            return this->type == Type::String ? this->text : empty;
        }

        ////// OBJECTS //////

        // The member of an object (null if it doesn't exist)
        const Json& operator[](std::string_view key) const {
            static const Json null;
            if (this->type != Type::Object)
                return null;
            for (std::size_t i = 0; i < this->keys.size(); i++) {
                if (this->keys[i] == key)
                    return this->items[i];
            }
            return null;
        }

        // The member of an object, created if it doesn't exist (a null value becomes an object)
        Json& operator[](std::string_view key) {
            if (this->type == Type::Null)
                this->type = Type::Object;
            for (std::size_t i = 0; i < this->keys.size(); i++) {
                if (this->keys[i] == key)
                    return this->items[i];
            }
            this->keys.emplace_back(key);
            this->items.emplace_back();
            return this->items.back();
        }

        bool Has(std::string_view key) const {
            if (this->type != Type::Object)
                return false;
            for (const std::string& k : this->keys) {
                if (k == key)
                    return true;
            }
            return false;
        }

        const std::vector<std::string>& Keys() const noexcept { return this->keys; }

        ////// ARRAYS //////

        std::size_t Size() const noexcept { return this->type == Type::Array || this->type == Type::Object ? this->items.size() : 0; }

        // The element of an array or the value of the member of an object by index
        const Json& At(std::size_t index) const {
            static const Json null;
            return index < this->items.size() ? this->items[index] : null;
        }

        // Add an element at the end of an array (a null value becomes an array)
        Json& Push(Json value) {
            if (this->type == Type::Null)
                this->type = Type::Array;
            this->items.push_back(std::move(value));
            return this->items.back();
        }

        ////// TEXT //////

        /**
         * Return the compact JSON text of the value
         */
        std::string Dump() const;

        /**
         * Parse a JSON text (nullopt if it's invalid)
         */
        static std::optional<Json> Parse(std::string_view text);

    };

}

#endif //LITESCRIPT_PLAYGROUND_TOOLS_JSONVALUE_HPP
