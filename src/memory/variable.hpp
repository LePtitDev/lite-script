#ifndef LITESCRIPT_LITESCRIPT_HPP
#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_MEMORY_VARIABLE_HPP
#define LITESCRIPT_MEMORY_VARIABLE_HPP

namespace LiteScript {

    class Object {

        Type& type;

        unsigned int size;
        std::allocator<char> data;

    public:

        Object();
        Object(Type&, unsigned int);

    };

}

#endif //LITESCRIPT_MEMORY_VARIABLE_HPP

#endif
