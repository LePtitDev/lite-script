#ifndef LITESCRIPT_TRACE_HPP
#define LITESCRIPT_TRACE_HPP

#include <litescript/object.hpp>

namespace LiteScript::Internal {

    // Call a function on each variable stored in a namer
    template <typename F>
    void TraceNamer(const Namer& namer, F&& visit) {
        visit(namer.global);
        visit(namer.current);
        for (const Variable& v : namer.scopes)
            visit(v);
    }

    // Call a function on each variable stored in a member list
    template <typename F>
    void TraceMembers(const MemberList& list, F&& visit) {
        for (const auto& member : list)
            visit(member.second);
    }

    // Call a function on each variable stored in the content of an object
    // (every variable must be visited exactly once, the garbage collector relies on it)
    template <typename F>
    void TraceData(const Data& data, F&& visit) {
        if (const auto * u = std::get_if<Undefined>(&data)) {
            visit(u->origin.container);
        }
        else if (const auto * r = std::get_if<Reference>(&data)) {
            visit(r->target);
        }
        else if (const auto * ch = std::get_if<Character>(&data)) {
            visit(ch->string);
        }
        else if (const auto * cb = std::get_if<Callback>(&data)) {
            if (const auto * f = std::get_if<Callback::ScriptFunction>(&cb->target))
                TraceNamer(f->scope, visit);
            visit(cb->self);
        }
        else if (const auto * a = std::get_if<Array>(&data)) {
            for (const Variable& v : a->indexed)
                visit(v);
            TraceMembers(a->named, visit);
        }
        else if (const auto * c = std::get_if<Class>(&data)) {
            for (const Variable& v : c->inherits)
                visit(v);
            TraceMembers(c->statics, visit);
            TraceMembers(c->members, visit);
            for (const Variable& v : c->operators)
                visit(v);
        }
        else if (const auto * o = std::get_if<ClassObject>(&data)) {
            visit(o->base);
            TraceMembers(o->members, visit);
        }
        else if (const auto * n = std::get_if<Namespace>(&data)) {
            TraceMembers(n->variables, visit);
        }
    }

}

#endif //LITESCRIPT_TRACE_HPP
