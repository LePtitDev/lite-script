#include <litescript/memory.hpp>
#include <litescript/operations.hpp>

#include <algorithm>

#include "trace.hpp"

LiteScript::Memory::Memory(std::size_t threshold) :
    gc_threshold(threshold), gc_min_threshold(threshold)
{
}

LiteScript::Memory::~Memory() {
    for (Variable& m : this->methods)
        m.Reset();
    // Break every reference between objects, the objects only referenced by other objects are destroyed
    std::vector<Variable> all;
    all.reserve(this->objects.size());
    for (const auto& obj : this->objects)
        all.emplace_back(*obj);
    for (Variable& v : all)
        v->data = Undefined{};
    all.clear();
    // The remaining objects are referenced by variables that outlive the memory,
    // they will be deleted by their last variable
    for (auto& obj : this->objects) {
        obj->memory = nullptr;
        obj.release();
    }
}

std::size_t LiteScript::Memory::CollectGarbage() {
    // Count the references that come from objects of the memory
    for (const auto& obj : this->objects) {
        obj->gc_references = obj->references;
        obj->gc_reachable = false;
    }
    for (const auto& obj : this->objects) {
        Internal::TraceData(obj->data, [this](const Variable& v) {
            if (v && v->memory == this)
                --v->gc_references;
        });
    }
    // The objects referenced from outside are the roots, mark everything reachable from them
    std::vector<Object *> work;
    for (const auto& obj : this->objects) {
        if (obj->gc_references > 0) {
            obj->gc_reachable = true;
            work.push_back(obj.get());
        }
    }
    while (!work.empty()) {
        Object * obj = work.back();
        work.pop_back();
        Internal::TraceData(obj->data, [this, &work](const Variable& v) {
            if (v && v->memory == this && !v->gc_reachable) {
                v->gc_reachable = true;
                work.push_back(v.Get());
            }
        });
    }
    // Keep the unreachable objects alive while their content is cleared, then release them
    std::vector<Variable> garbage;
    for (const auto& obj : this->objects) {
        if (!obj->gc_reachable)
            garbage.emplace_back(*obj);
    }
    for (Variable& v : garbage)
        v->data = Undefined{};
    std::size_t count = garbage.size();
    garbage.clear();
    return count;
}

std::size_t LiteScript::Memory::CollectGarbageIfNeeded() {
    if (this->gc_threshold == 0 || this->objects.size() < this->gc_threshold)
        return 0;
    std::size_t count = this->CollectGarbage();
    this->gc_threshold = std::max(this->gc_min_threshold, this->objects.size() * 2);
    return count;
}

void LiteScript::Memory::SetGarbageCollectorThreshold(std::size_t threshold) noexcept {
    this->gc_threshold = threshold;
    this->gc_min_threshold = threshold;
}

LiteScript::Variable LiteScript::Memory::Create(Data data) {
    std::unique_ptr<Object> obj(new Object(*this, std::move(data)));
    obj->slot = this->objects.size();
    Variable result(*obj);
    this->objects.push_back(std::move(obj));
    return result;
}

LiteScript::Variable LiteScript::Memory::CreateArray(std::initializer_list<Variable> values) {
    Array array;
    array.indexed.reserve(values.size());
    for (const Variable& v : values)
        array.indexed.push_back(Copy(*this, v));
    return this->Create(std::move(array));
}

LiteScript::Variable LiteScript::Memory::CreateArray(std::initializer_list<std::pair<std::string_view, Variable>> values) {
    Array array;
    for (const auto& [name, v] : values)
        array.named.Set(name, Copy(*this, v));
    return this->Create(std::move(array));
}

LiteScript::Variable LiteScript::Memory::CreateCallback(NativeFunction::Signature function) {
    Callback callback;
    callback.target = std::make_shared<const NativeFunction>(NativeFunction{ std::string(), std::move(function) });
    return this->Create(std::move(callback));
}

LiteScript::Variable LiteScript::Memory::CreateCallback(std::string_view name) {
    auto function = this->FindFunction(name);
    if (function == nullptr)
        return this->CreateUndefined();
    Callback callback;
    callback.target = std::move(function);
    return this->Create(std::move(callback));
}

LiteScript::Variable LiteScript::Memory::RegisterFunction(std::string name, NativeFunction::Signature function) {
    auto native = std::make_shared<const NativeFunction>(NativeFunction{ name, std::move(function) });
    this->natives.insert_or_assign(std::move(name), native);
    Callback callback;
    callback.target = std::move(native);
    return this->Create(std::move(callback));
}

std::shared_ptr<const LiteScript::NativeFunction> LiteScript::Memory::FindFunction(std::string_view name) const {
    auto it = this->natives.find(std::string(name));
    return it != this->natives.end() ? it->second : nullptr;
}

void LiteScript::Memory::SetMethods(Type type, Variable namespace_object) {
    this->methods[static_cast<std::size_t>(type)] = std::move(namespace_object);
}

LiteScript::Variable LiteScript::Memory::FindMethod(Type type, std::string_view name) const {
    const Variable& table = this->methods[static_cast<std::size_t>(type)];
    if (!table.Is<Namespace>())
        return Variable();
    Variable method = table.As<Namespace>().variables.Find(name);
    return method.Is<Callback>() ? method : Variable();
}

void LiteScript::Memory::Destroy(Object * object) noexcept {
    if (this->releasing) {
        this->pending.push_back(object);
        return;
    }
    this->releasing = true;
    this->pending.push_back(object);
    while (!this->pending.empty()) {
        Object * obj = this->pending.back();
        this->pending.pop_back();
        // Remove the object from the list (the last object takes its place)
        std::size_t slot = obj->slot;
        std::unique_ptr<Object> owned = std::move(this->objects[slot]);
        if (slot + 1 != this->objects.size()) {
            this->objects[slot] = std::move(this->objects.back());
            this->objects[slot]->slot = slot;
        }
        this->objects.pop_back();
        // Destroying the content can release other objects, they are added to the pending list
        owned.reset();
    }
    this->releasing = false;
}
