#ifndef LITESCRIPT_LITESCRIPT_HPP
#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_MEMORY_TYPE_HPP
#define LITESCRIPT_MEMORY_TYPE_HPP

namespace LiteScript {

    class Type {

    public:

        static Type& NIL;
        static Type& BOOLEAN;
        static Type& NUMBER;
        static Type& STRING;

    private:

        unsigned int id;
        std::string name;

    public:

        Type(const char *);

        unsigned int GetID() const;
        const char * GetName() const;

        virtual Object CreateObject() = 0;
        virtual Object Convert(const Object&, const Type&) const;
        virtual Object& AssignObject(Object&);

        bool operator==(const Type&) const;
        bool operator!=(const Type&) const;

        /****** OPERATORS ******/

        virtual void ODestroy(Object&);

        virtual Object& OAssign(Object&, const Object&) const;

        virtual Object OUnaryPlus(const Object&) const;
        virtual Object OUnaryMinus(const Object&) const;
        virtual Object& OPreIncrement(Object&) const;
        virtual Object OPostIncrement(Object&) const;
        virtual Object& OPreDecrement(Object&) const;
        virtual Object OPostDecrement(Object&) const;

        virtual Object OAdd(const Object&, const Object&) const;
        virtual Object OSubstract(const Object&, const Object&) const;
        virtual Object OMultiply(const Object&, const Object&) const;
        virtual Object ODivide(const Object&, const Object&) const;
        virtual Object OModulo(const Object&, const Object&) const;

        virtual Object OEqual(const Object&, const Object&) const;
        virtual Object ONotEqual(const Object&, const Object&) const;
        virtual Object OGreater(const Object&, const Object&) const;
        virtual Object OLess(const Object&, const Object&) const;
        virtual Object OGreaterOrEqual(const Object&, const Object&) const;
        virtual Object OLessOrEqual(const Object&, const Object&) const;

        virtual Object OLogicalNot(const Object&) const;
        virtual Object OLogicalAnd(const Object&, const Object&) const;
        virtual Object OLogicalOr(const Object&, const Object&) const;

        virtual Object OBitwiseNot(const Object&) const;
        virtual Object OBitwiseAnd(const Object&, const Object&) const;
        virtual Object OBitwiseOr(const Object&, const Object&) const;
        virtual Object OBitwiseXor(const Object&, const Object&) const;
        virtual Object OLeftShift(const Object&, const Object&) const;
        virtual Object ORightShift(const Object&, const Object&) const;

        virtual Object& OAddAndAssign(Object&, const Object&) const;
        virtual Object& OSubstractAndAssign(Object&, const Object&) const;
        virtual Object& OMultiplyAndAssign(Object&, const Object&) const;
        virtual Object& ODivideAndAssign(Object&, const Object&) const;

        virtual Object& OArray(Object&, const Object&) const;
        virtual Object& OMember(Object&, const char *) const;

        virtual Object OCall(Object&, std::vector<std::unique_ptr<Object>>&) const;

        virtual std::string ToString(const Object&) const;

    };

}

#endif //LITESCRIPT_MEMORY_TYPE_HPP

#endif
