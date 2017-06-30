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

        // Indicate if the value is null
        bool is_null;

    public:

        // Indicate if the value is null
        const bool& isNull;

        /////////////////////////
        ////// CONSTRUCTORS /////
        /////////////////////////

        /**
         * Null constructor
         */
        Nullable() : is_null(true), isNull(is_null) {}

        /**
         * Not null constructor
         *
         * @param t The value
         */
        Nullable(const T& t) : is_null(false), isNull(is_null) {
            if (!this->is_null) {
                std::allocator<T> allocator;
                allocator.construct((T *)this->data, t);
            }
        }

        /**
         * Copy constructor
         *
         * @param v The nullable copy
         */
        Nullable(const Nullable<T>& v) : is_null(v.is_null), isNull(is_null) {
            if (!this->is_null) {
                std::allocator<T> allocator;
                allocator.construct((T *)this->data, *(const T *)v.data);
            }
        }

        /**
         * Destructor of the nullable value
         */
        ~Nullable() {
            this->Nullify();
        }

        /////////////////////
        ////// METHODS //////
        /////////////////////

        void Nullify() {
            if (!this->is_null) {
                std::allocator<T> allocator;
                allocator.destroy((T *)this->data);
            }
            this->is_null = true;
        }

        // The assign operator
        Nullable<T>& operator=(const T& t) {
            this->Nullify();
            if (!(this->is_null = false)) {
                std::allocator<T> allocator;
                allocator.construct((T *)this->data, t);
            }
        }

        // The assign operator
        Nullable<T>& operator=(const Nullable<T>& v) {
            this->Nullify();
            if (!(this->is_null = v.is_null)) {
                std::allocator<T> allocator;
                allocator.construct((T *)this->data, *(const T *)v.data);
            }
        }

        // The converter operator
        operator T() { return T(*(const T *)this->data); }

        // The pointer operator
        T& operator*() { return *(T *)this->data; }

        // The pointer operator
        const T& operator*() const { return *(T *)this->data; }

        // The pointer operator
        T* operator->() { return (T *)this->data; }

        // The pointer operator
        const T* operator->() const { return (T *)this->data; }

    };

}

#endif //LITESCRIPT_NULLABLE_HPP
