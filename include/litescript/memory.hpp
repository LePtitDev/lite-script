#ifndef LITESCRIPT_MEMORY_HPP
#define LITESCRIPT_MEMORY_HPP

#include <array>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "object.hpp"

namespace LiteScript {

    // The memory that owns the objects
    // Objects are destroyed as soon as they are not referenced anymore, objects that reference each
    // other (cycles) are destroyed by the garbage collector. The garbage collector never destroys an
    // object referenced by a variable that isn't stored in another object (variables of the
    // application, of the script states...), so it can be executed at any time.
    class Memory {

        friend class Object;

        // The objects of the memory
        std::vector<std::unique_ptr<Object>> objects;

        // Objects waiting to be destroyed (avoid recursive destructions)
        std::vector<Object *> pending;

        // Indicate if objects are being destroyed
        bool releasing = false;

        // The registered native functions
        std::unordered_map<std::string, std::shared_ptr<const NativeFunction>> natives;

        // Number of objects that triggers the automatic garbage collection (0 to disable it)
        std::size_t gc_threshold;

        // Minimum value of the threshold
        std::size_t gc_min_threshold;

        // The methods of each type of values (namespaces of callbacks, empty if not defined)
        std::array<Variable, TYPE_COUNT> methods;

    public:

        /**
         * Constructor of an empty memory
         *
         * @param gc_threshold Number of objects from which the automatic garbage collector
         *                     is executed (0 to disable the automatic garbage collector)
         */
        explicit Memory(std::size_t gc_threshold = 4096);

        /**
         * Destructor of the memory, destroys every object
         * (the variables that still exist after it remain valid but must not be used with a state)
         */
        ~Memory();

        Memory(const Memory&) = delete;
        Memory& operator=(const Memory&) = delete;

        /**
         * Return the number of objects in the memory
         */
        std::size_t Count() const noexcept { return this->objects.size(); }

        /**
         * Destroy the objects that are not reachable from outside the memory
         *
         * @return The number of destroyed objects
         */
        std::size_t CollectGarbage();

        /**
         * Execute the garbage collector if the number of objects reached the threshold
         * (called by the states between instructions)
         *
         * @return The number of destroyed objects
         */
        std::size_t CollectGarbageIfNeeded();

        /**
         * Change the threshold of the automatic garbage collector (0 to disable it)
         */
        void SetGarbageCollectorThreshold(std::size_t threshold) noexcept;

        ////// OBJECTS CREATION //////

        /**
         * Create an object
         *
         * @param data The content of the object
         * @return A variable that references the object
         */
        Variable Create(Data data = Null{});

        Variable CreateUndefined() { return this->Create(Undefined{}); }
        Variable CreateNull() { return this->Create(Null{}); }
        Variable CreateBoolean(bool value) { return this->Create(value); }
        Variable CreateNumber(Number value) { return this->Create(value); }
        Variable CreateString(std::string_view utf8) { return this->Create(String(utf8)); }
        Variable CreateNamespace() { return this->Create(Namespace{}); }

        /**
         * Create an array that contains copies of the values
         */
        Variable CreateArray(std::initializer_list<Variable> values = {});

        /**
         * Create an array with named elements that contains copies of the values
         */
        Variable CreateArray(std::initializer_list<std::pair<std::string_view, Variable>> values);

        /**
         * Create a callback that calls a function of the application
         * (an unregistered function can't be saved with a state)
         */
        Variable CreateCallback(NativeFunction::Signature function);

        /**
         * Create a callback that calls a registered function of the application
         *
         * @param name The name of the registered function
         * @return The callback or an undefined object if the function is unknown
         */
        Variable CreateCallback(std::string_view name);

        ////// NATIVE FUNCTIONS //////

        /**
         * Register a function of the application
         * The name is used to restore the callbacks when a state is loaded
         *
         * @param name The unique name of the function
         * @param function The function
         * @return A callback that calls the function
         */
        Variable RegisterFunction(std::string name, NativeFunction::Signature function);

        /**
         * Find a registered function
         *
         * @return The function or nullptr if not found
         */
        std::shared_ptr<const NativeFunction> FindFunction(std::string_view name) const;

        ////// METHODS OF THE TYPES //////

        /**
         * Define the methods of the values of a type ("value.name(...)" calls the callback of the
         * namespace with "this" bound to the value, the elements of an array have the priority)
         *
         * @param type The type of the values
         * @param namespace_object A namespace of callbacks (an empty variable removes the methods)
         */
        void SetMethods(Type type, Variable namespace_object);

        /**
         * Find a method of a type
         *
         * @return The callback or an empty variable if not found
         */
        Variable FindMethod(Type type, std::string_view name) const;

    private:

        /**
         * Destroy an object that isn't referenced anymore
         */
        void Destroy(Object * object) noexcept;

    };

}

#endif //LITESCRIPT_MEMORY_HPP
