#ifndef LITESCRIPT_LITESCRIPT_HPP
#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_MEMORY_VARIABLE_HPP
#define LITESCRIPT_MEMORY_VARIABLE_HPP

namespace LiteScript {

    class Object {

        Type& type;

        unsigned int size;
        void * data;
        std::allocator<char> allocator;

    public:

        Object();
        Object(Type&, unsigned int);
        Object(const Object&);
        ~Object();

        Type& GetType() const;

        template<typename T>
        T& GetData() const { return *(T*)this->data; }

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

        bool operator==(const Object&) const;
        bool operator!=(const Object&) const;
        bool operator>(const Object&) const;
        bool operator<(const Object&) const;
        bool operator>=(const Object&) const;
        bool operator<=(const Object&) const;

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

        std::string ToString() const;

    };

}

#endif //LITESCRIPT_MEMORY_VARIABLE_HPP

#endif
