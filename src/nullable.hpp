/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/*
    Copyright (C) 2017 LePtitDev
    All rights reserved.

    This software may be modified and distributed under the terms
    of the BSD license.  See the LICENSE file for details.

    Author: Arthur Ferré <leptitdev.com>
*/
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#ifndef LITESCRIPT_NULLABLE_HPP
#define LITESCRIPT_NULLABLE_HPP

#include <memory>

namespace LiteScript {

    // Nullable object (the object need a copy constructor)
    template <class T>
    class Nullable {

        ////////////////////////
        ////// ATTRIBUTES //////
        ////////////////////////

        // The internal data of the nullable value
        char data[sizeof(T)];

    public:

        // Indicate if the value is null
        const bool isNull;

        /////////////////////////
        ////// CONSTRUCTORS /////
        /////////////////////////

        /**
         * Null constructor
         */
        Nullable() : isNull(true) {}

        /**
         * Not null constructor
         *
         * @param t The value
         */
        Nullable(const T& t) : isNull(false) {
            if (!this->isNull) {
                std::allocator<T> allocator;
                allocator.construct((T *)this->data, t);
            }
        }

        /**
         * Copy constructor
         *
         * @param v The nullable copy
         */
        Nullable(const Nullable<T>& v) : isNull(v.isNull) {
            if (!this->isNull) {
                std::allocator<T> allocator;
                allocator.construct((T *)this->data, *(const T *)v.data);
            }
        }

        /**
         * Destructor of the nullable value
         */
        ~Nullable() {
            if (this->isNull) {
                std::allocator<T> allocator;
                allocator.destroy((T *)this->data);
            }
        }

        /////////////////////
        ////// METHODS //////
        /////////////////////

        // The converter operator
        operator T() {
            return T(*(const T *)this->data);
        }

    };

}

#endif //LITESCRIPT_NULLABLE_HPP
