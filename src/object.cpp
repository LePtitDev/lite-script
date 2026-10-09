#include <litescript/memory.hpp>

std::string_view LiteScript::GetTypeName(Type type) noexcept {
    switch (type) {
        case Type::Undefined: return "undefined";
        case Type::Null: return "null";
        case Type::Boolean: return "boolean";
        case Type::Number: return "number";
        case Type::String: return "string";
        case Type::Character: return "character";
        case Type::Callback: return "callback";
        case Type::Array: return "array";
        case Type::Class: return "class";
        case Type::ClassObject: return "class object";
        case Type::Namespace: return "namespace";
    }
    return "unknown";
}

namespace {

    // Maximum number of memory blocks kept by a thread
    constexpr std::size_t MAX_CACHED_BLOCKS = 4096;

    // The memory blocks of destroyed objects, reused by the next objects of the thread
    struct BlockCache {
        std::vector<void *> blocks;

        ~BlockCache();
    };

    thread_local BlockCache block_cache;

    // Indicate if the cache of the thread has been destroyed (objects can be destroyed after it)
    thread_local bool block_cache_destroyed = false;

    BlockCache::~BlockCache() {
        for (void * block : this->blocks)
            ::operator delete(block);
        this->blocks.clear();
        block_cache_destroyed = true;
    }

}

void * LiteScript::Object::operator new(std::size_t size) {
    if (size == sizeof(Object) && !block_cache_destroyed && !block_cache.blocks.empty()) {
        void * block = block_cache.blocks.back();
        block_cache.blocks.pop_back();
        return block;
    }
    return ::operator new(size);
}

void LiteScript::Object::operator delete(void * block, std::size_t size) noexcept {
    if (block == nullptr)
        return;
    if (size == sizeof(Object) && !block_cache_destroyed && block_cache.blocks.size() < MAX_CACHED_BLOCKS) {
        try {
            block_cache.blocks.push_back(block);
            return;
        }
        catch (...) {
        }
    }
    ::operator delete(block);
}

void LiteScript::Object::ReleaseCachedBlocks() noexcept {
    if (block_cache_destroyed)
        return;
    for (void * block : block_cache.blocks)
        ::operator delete(block);
    block_cache.blocks.clear();
    block_cache.blocks.shrink_to_fit();
}

void LiteScript::Object::Release() noexcept {
    if (this->memory != nullptr)
        this->memory->Destroy(this);
    else
        delete this;
}

////// MEMBER LIST //////

std::optional<std::size_t> LiteScript::MemberList::IndexOf(std::string_view name) const noexcept {
    for (std::size_t i = 0, sz = this->items.size(); i < sz; i++) {
        if (this->items[i].first == name)
            return i;
    }
    return std::nullopt;
}

LiteScript::Variable LiteScript::MemberList::Find(std::string_view name) const {
    if (auto index = this->IndexOf(name))
        return this->items[*index].second;
    return Variable();
}

void LiteScript::MemberList::Set(std::string_view name, Variable value) {
    if (auto index = this->IndexOf(name))
        this->items[*index].second = std::move(value);
    else
        this->items.emplace_back(std::string(name), std::move(value));
}

////// NAMER //////

LiteScript::Variable LiteScript::Namer::Find(std::string_view name) const {
    for (auto it = this->scopes.rbegin(); it != this->scopes.rend(); ++it) {
        Variable v;
        if (it->Is<Namespace>())
            v = it->As<Namespace>().variables.Find(name);
        else if (it->Is<Class>())
            v = it->As<Class>().statics.Find(name);
        if (v)
            return v;
    }
    if (this->current.Is<Namespace>()) {
        if (Variable v = this->current.As<Namespace>().variables.Find(name))
            return v;
    }
    if (this->global.Is<Namespace>() && this->global != this->current)
        return this->global.As<Namespace>().variables.Find(name);
    return Variable();
}

void LiteScript::Namer::Declare(std::string_view name, Variable value) {
    for (auto it = this->scopes.rbegin(); it != this->scopes.rend(); ++it) {
        if (it->Is<Namespace>()) {
            it->As<Namespace>().variables.Set(name, std::move(value));
            return;
        }
    }
    if (this->current.Is<Namespace>())
        this->current.As<Namespace>().variables.Set(name, std::move(value));
    else if (this->global.Is<Namespace>())
        this->global.As<Namespace>().variables.Set(name, std::move(value));
}

void LiteScript::Namer::Push(Variable scope) {
    if (scope.Is<Namespace>() || scope.Is<Class>())
        this->scopes.push_back(std::move(scope));
}

void LiteScript::Namer::Pop(const Variable& scope) {
    if (!this->scopes.empty() && this->scopes.back() == scope)
        this->scopes.pop_back();
}

////// CALLBACK //////

bool LiteScript::Callback::SameTarget(const Callback& c) const noexcept {
    if (this->target.index() != c.target.index())
        return false;
    if (const auto * f = std::get_if<ScriptFunction>(&this->target)) {
        const auto& g = std::get<ScriptFunction>(c.target);
        return f->list == g.list && f->line == g.line;
    }
    return std::get<1>(this->target) == std::get<1>(c.target);
}

////// CLASS //////

void LiteScript::Class::Inherit(const Variable& parent) {
    if (!parent.Is<Class>())
        return;
    const Class& c = parent.As<Class>();
    if (&c == this)
        return;
    this->inherits.push_back(parent);
    for (const auto& [name, value] : c.statics)
        this->statics.Set(name, value);
    for (const auto& [name, value] : c.members)
        this->members.Set(name, value);
    for (std::size_t i = 0; i < OPERATOR_COUNT; i++) {
        if (c.operators[i])
            this->operators[i] = c.operators[i];
    }
}

bool LiteScript::Class::DefineConstructor(std::string_view name) {
    if (!this->members.Find(name).Is<Callback>())
        return false;
    this->constructor = name;
    return true;
}

LiteScript::Variable LiteScript::Class::GetConstructor() const {
    if (this->constructor.empty())
        return Variable();
    Variable v = this->members.Find(this->constructor);
    return v.Is<Callback>() ? v : Variable();
}

////// CLASS OBJECT //////

const LiteScript::Class * LiteScript::ClassObject::GetClass() const noexcept {
    return this->base.Is<Class>() ? &this->base.As<Class>() : nullptr;
}
