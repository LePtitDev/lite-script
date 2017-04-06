#include "variable.hpp"

LiteScript::Object::Object() :
    type(Type::NIL), size(0)
{

}

LiteScript::Object::Object(Type & t, unsigned int n) :
    type(t), size(n)
{
    if (n > 0)
        this->data.allocate(n);
}