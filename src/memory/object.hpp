#ifndef LITESCRIPT_LITESCRIPT_HPP
#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_MEMORY_VARIABLE_HPP
#define LITESCRIPT_MEMORY_VARIABLE_HPP

namespace LiteScript {

    extern std::allocator<char> ObjectAllocator;

    class Object {

    public:

        static Object UNDEFINED;

    private:

        Type * type;

        unsigned int size;
        void * data;

    public:

        Object();
        Object(Type&, unsigned int);
        Object(const Object&);
        ~Object();

        Type& GetType() const;

        template<typename T>
        T& GetData() { return *(T*)this->data; }
        template<typename T>
        const T& GetData() const { return *(T*)this->data; }

        Object Convert(const Type&) const;
        Object& Reassign(Type&, unsigned int);

        Object& operator=(const Object&);

        Object operator+() const;
        Object operator-() const;
        Object& operator++();
        Object operator++(int);
        Object& operator--();
        Object operator--(int);

        Object operator+(const Object&) const;
        Object operator-(const Object&) const;
        Object operator*(const Object&) const;
        Object operator/(const Object&) const;
        Object operator%(const Object&) const;

        Object operator==(const Object&) const;
        Object operator!=(const Object&) const;
        Object operator>(const Object&) const;
        Object operator<(const Object&) const;
        Object operator>=(const Object&) const;
        Object operator<=(const Object&) const;

        Object operator!() const;
        Object operator&&(const Object&) const;
        Object operator||(const Object&) const;

        Object operator~() const;
        Object operator&(const Object&) const;
        Object operator|(const Object&) const;
        Object operator^(const Object&) const;
        Object operator<<(const Object&) const;
        Object operator>>(const Object&) const;

        Object& operator+=(const Object&);
        Object& operator-=(const Object&);
        Object& operator*=(const Object&);
        Object& operator/=(const Object&);

        Object& operator[](const Object&);
        Object& GetMember(const char *);

        Object operator()(std::vector<std::unique_ptr<Object>>&);

        operator std::string() const;

    };

}

std::ostream& operator<<(std::ostream&, const LiteScript::Object&);

#endif //LITESCRIPT_MEMORY_VARIABLE_HPP

#endif
