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

#ifndef LITESCRIPT_MEMORY_OBJECT_HPP
#define LITESCRIPT_MEMORY_OBJECT_HPP

namespace LiteScript {

    // Class declarations
    class Type;
    class Memory;

    // Basic class of an object
    class Object {

        // Allocator of each objects
        static std::allocator<char> allocator;

        ///////////////////////////////
        ////// OBJECT ATTRIBUTES //////
        ///////////////////////////////

        // Type of the object
        Type * type;

        // Size of internal datas in bytes
        unsigned int size;

        // Pointer to internal data (nullptr if size equal to 0)
        void * data;

    public:

        // The ID of the object
        const unsigned int ID;

        // The memory that contain the object
        Memory& memory;

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor of an object
         * The type of this object is "null"
         *
         * @param mem The memory that contain the object
         * @param id The ID of the object
         */
        Object(Memory& mem, unsigned int id);

        /**
         * Constructor of an object by copy
         *
         * @param object The object to be copied
         */
        Object(const Object& object);

        /**
         * Destructor of an object
         * If size of datas is more than zero, unallocate datas
         */
        ~Object();

        /////////////////////
        ////// METHODS //////
        /////////////////////

        /**
         * Clear the content of the object
         */
        void Clear();

        /**
         * Get the type of the object
         *
         * @return The type of the object
         */
        Type& GetType() const;

        /**
         * Get the internal datas by cast
         * Example:
         *   // Create an object of type "boolean"
         *   Object object = Type::BOOLEAN.CreateObject();
         *   // Display the content of the object (see the datas type casts by each object type)
         *   cout << object.GetData<bool>() << endl;
         *
         * @return The casted datas
         */
        template<typename T>
        T& GetData() { return *(T*)this->data; }

        /**
         * Get the internal datas by cast (constant method)
         * Example:
         *   // Create a constant object of type "boolean"
         *   const Object object = Type::BOOLEAN.CreateObject();
         *   // Display the content of the object (see the datas type casts by each object type)
         *   cout << object.GetData<bool>() << endl;
         *
         * @return The casted datas (constant)
         */
        template<typename T>
        const T& GetData() const { return *(T*)this->data; }

        /**
         * Reassign datas and type
         * Destroy and reallocate datas and type
         *
         * @param type The new type of the object
         * @param size The size of new datas of the object
         */
        Object& Reassign(Type& type, unsigned int size);

    };

}

#endif //LITESCRIPT_MEMORY_VARIABLE_HPP