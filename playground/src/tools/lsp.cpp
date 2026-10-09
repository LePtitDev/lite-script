#include "tools.hpp"

#include "api.hpp"
#include "jsonvalue.hpp"
#include "transport.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <litescript/litescript.hpp>

namespace {

    using Tools::ApiEntry;
    using Tools::Json;
    using Analysis = LiteScript::Script::Analysis;
    using SymbolKind = Analysis::SymbolKind;
    constexpr std::size_t NONE = Analysis::NONE;

    // Kinds of the protocol
    namespace Lsp {
        constexpr int SEVERITY_ERROR = 1, SEVERITY_WARNING = 2;
        constexpr int COMPLETION_METHOD = 2, COMPLETION_FUNCTION = 3, COMPLETION_CONSTRUCTOR = 4, COMPLETION_FIELD = 5,
                      COMPLETION_VARIABLE = 6, COMPLETION_CLASS = 7, COMPLETION_MODULE = 9, COMPLETION_PROPERTY = 10,
                      COMPLETION_KEYWORD = 14, COMPLETION_SNIPPET = 15, COMPLETION_CONSTANT = 21, COMPLETION_OPERATOR = 24;
        constexpr int SYMBOL_NAMESPACE = 3, SYMBOL_CLASS = 5, SYMBOL_METHOD = 6, SYMBOL_FIELD = 8, SYMBOL_CONSTRUCTOR = 9,
                      SYMBOL_FUNCTION = 12, SYMBOL_VARIABLE = 13, SYMBOL_OPERATOR = 25;
        constexpr int ERROR_METHOD_NOT_FOUND = -32601, ERROR_REQUEST_FAILED = -32803;
    }

    bool IsNameChar(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '$';
    }

    bool IsBlank(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    // Indicate for each byte of a code if it's in a comment or a string
    std::vector<bool> MaskCommentsAndStrings(std::string_view text) {
        std::vector<bool> mask(text.size() + 1, false);
        std::size_t i = 0;
        while (i < text.size()) {
            char c = text[i];
            if (c == '/' && i + 1 < text.size() && text[i + 1] == '/') {
                while (i < text.size() && text[i] != '\n')
                    mask[i++] = true;
            }
            else if (c == '/' && i + 1 < text.size() && text[i + 1] == '*') {
                std::size_t end = text.find("*/", i + 2);
                end = end == std::string_view::npos ? text.size() : end + 2;
                while (i < end)
                    mask[i++] = true;
                if (end == text.size())
                    mask[i] = true;
            }
            else if (c == '"' || c == '\'') {
                mask[i++] = true;
                while (i < text.size() && text[i] != c && text[i] != '\n') {
                    if (text[i] == '\\' && i + 1 < text.size())
                        mask[i++] = true;
                    mask[i++] = true;
                }
                if (i < text.size() && text[i] == c)
                    i++;
                else
                    mask[i] = true;
            }
            else {
                i++;
            }
        }
        return mask;
    }

    // An open document
    struct Document {
        std::string uri;
        std::string text;
        std::int64_t version = 0;
        std::vector<std::size_t> line_starts;
        Analysis analysis;
        LiteScript::Script::Result result;
        // The names declared by the last code without error (for the completion when there is an error)
        std::set<std::string> last_names;

        void Update(std::string code) {
            this->text = std::move(code);
            this->line_starts.assign(1, 0);
            for (std::size_t i = 0; i < this->text.size(); i++) {
                if (this->text[i] == '\n')
                    this->line_starts.push_back(i + 1);
            }
            this->result = LiteScript::Script::Compile(this->text, &this->analysis);
            this->result.instructions.clear();
            if (this->result) {
                this->last_names.clear();
                for (const auto& symbol : this->analysis.symbols) {
                    if (symbol.kind != SymbolKind::Namespace)
                        this->last_names.insert(symbol.name);
                }
            }
        }

        // Convert a position of the protocol (UTF-16 column) into an offset
        std::size_t OffsetOf(const Json& position) const {
            std::size_t line = static_cast<std::size_t>(std::max<std::int64_t>(0, position["line"].GetInt()));
            std::int64_t character = position["character"].GetInt();
            if (line >= this->line_starts.size())
                return this->text.size();
            std::size_t offset = this->line_starts[line];
            while (character > 0 && offset < this->text.size() && this->text[offset] != '\n') {
                unsigned char c = static_cast<unsigned char>(this->text[offset]);
                std::size_t length = c < 0x80 ? 1 : c < 0xE0 ? 2 : c < 0xF0 ? 3 : 4;
                character -= length == 4 ? 2 : 1;
                offset += length;
            }
            return std::min(offset, this->text.size());
        }

        // Convert an offset into a position of the protocol
        Json PositionOf(std::size_t offset) const {
            offset = std::min(offset, this->text.size());
            auto it = std::upper_bound(this->line_starts.begin(), this->line_starts.end(), offset) - 1;
            std::int64_t character = 0;
            for (std::size_t i = *it; i < offset; i++) {
                unsigned char c = static_cast<unsigned char>(this->text[i]);
                if ((c & 0xC0) == 0x80)
                    continue;
                character += c >= 0xF0 ? 2 : 1;
            }
            Json res;
            res["line"] = static_cast<std::int64_t>(it - this->line_starts.begin());
            res["character"] = character;
            return res;
        }

        Json RangeOf(std::size_t start, std::size_t end) const {
            Json res;
            res["start"] = this->PositionOf(start);
            res["end"] = this->PositionOf(end);
            return res;
        }

        Json RangeOf(const Analysis::Range& range) const {
            return this->RangeOf(range.start.offset, range.end.offset);
        }

        Json LocationOf(const Analysis::Range& range) const {
            Json res;
            res["uri"] = this->uri;
            res["range"] = this->RangeOf(range);
            return res;
        }
    };

    class LanguageServer {

        Tools::Transport& transport;
        std::map<std::string, Document> documents;
        bool shutdown = false;
        // Report the names that aren't declared
        bool unknown_variables = true;

    public:

        explicit LanguageServer(Tools::Transport& t) : transport(t) {}

        int Run() {
            while (auto message = this->transport.Read()) {
                const std::string& method = (*message)["method"].GetString();
                if (method == "exit")
                    return this->shutdown ? 0 : 1;
                bool request = message->Has("id");
                try {
                    Json result = this->Handle(method, (*message)["params"], request);
                    if (request)
                        this->Respond((*message)["id"], std::move(result));
                }
                catch (const std::pair<int, std::string>& error) {
                    if (request)
                        this->RespondError((*message)["id"], error.first, error.second);
                }
                catch (const std::exception& error) {
                    if (request)
                        this->RespondError((*message)["id"], Lsp::ERROR_REQUEST_FAILED, error.what());
                }
            }
            return 1;
        }

    private:

        ////// MESSAGES //////

        void Respond(const Json& id, Json result) {
            Json message;
            message["jsonrpc"] = "2.0";
            message["id"] = id;
            message["result"] = std::move(result);
            this->transport.Write(message);
        }

        void RespondError(const Json& id, int code, const std::string& text) {
            Json message;
            message["jsonrpc"] = "2.0";
            message["id"] = id;
            message["error"]["code"] = code;
            message["error"]["message"] = text;
            this->transport.Write(message);
        }

        void Notify(const std::string& method, Json params) {
            Json message;
            message["jsonrpc"] = "2.0";
            message["method"] = method;
            message["params"] = std::move(params);
            this->transport.Write(message);
        }

        Json Handle(const std::string& method, const Json& params, bool request) {
            if (method == "initialize")
                return this->Initialize(params);
            if (method == "shutdown") {
                this->shutdown = true;
                return Json();
            }
            if (method == "textDocument/didOpen") {
                const Json& doc = params["textDocument"];
                Document& d = this->documents[doc["uri"].GetString()];
                d.uri = doc["uri"].GetString();
                d.version = doc["version"].GetInt();
                d.Update(doc["text"].GetString());
                this->PublishDiagnostics(d);
                return Json();
            }
            if (method == "textDocument/didChange") {
                auto it = this->documents.find(params["textDocument"]["uri"].GetString());
                const Json& changes = params["contentChanges"];
                if (it != this->documents.end() && changes.Size() > 0) {
                    it->second.version = params["textDocument"]["version"].GetInt();
                    // Full synchronization: the last change contains the whole text
                    it->second.Update(changes.At(changes.Size() - 1)["text"].GetString());
                    this->PublishDiagnostics(it->second);
                }
                return Json();
            }
            if (method == "textDocument/didClose") {
                std::string uri = params["textDocument"]["uri"].GetString();
                this->documents.erase(uri);
                Json clear;
                clear["uri"] = uri;
                clear["diagnostics"] = Json::MakeArray();
                this->Notify("textDocument/publishDiagnostics", std::move(clear));
                return Json();
            }
            if (method == "workspace/didChangeConfiguration") {
                const Json& settings = params["settings"]["litescript"];
                if (settings["diagnostics"].Has("unknownVariables")) {
                    this->unknown_variables = settings["diagnostics"]["unknownVariables"].GetBool(true);
                    for (auto& [uri, doc] : this->documents)
                        this->PublishDiagnostics(doc);
                }
                return Json();
            }
            if (method.rfind("textDocument/", 0) == 0 && request) {
                auto it = this->documents.find(params["textDocument"]["uri"].GetString());
                if (it == this->documents.end())
                    return Json();
                Document& doc = it->second;
                if (method == "textDocument/completion")
                    return this->Completion(doc, doc.OffsetOf(params["position"]));
                if (method == "textDocument/hover")
                    return this->Hover(doc, doc.OffsetOf(params["position"]));
                if (method == "textDocument/signatureHelp")
                    return this->SignatureHelp(doc, doc.OffsetOf(params["position"]));
                if (method == "textDocument/definition")
                    return this->Definition(doc, doc.OffsetOf(params["position"]));
                if (method == "textDocument/references")
                    return this->References(doc, doc.OffsetOf(params["position"]), params["context"]["includeDeclaration"].GetBool(true));
                if (method == "textDocument/documentHighlight")
                    return this->Highlights(doc, doc.OffsetOf(params["position"]));
                if (method == "textDocument/prepareRename")
                    return this->PrepareRename(doc, doc.OffsetOf(params["position"]));
                if (method == "textDocument/rename")
                    return this->Rename(doc, doc.OffsetOf(params["position"]), params["newName"].GetString());
                if (method == "textDocument/documentSymbol")
                    return this->DocumentSymbols(doc);
                if (method == "textDocument/foldingRange")
                    return this->FoldingRanges(doc);
            }
            if (request)
                throw std::pair<int, std::string>(Lsp::ERROR_METHOD_NOT_FOUND, "unknown method " + method);
            return Json();
        }

        Json Initialize(const Json& params) {
            const Json& options = params["initializationOptions"];
            if (options.Has("unknownVariables"))
                this->unknown_variables = options["unknownVariables"].GetBool(true);
            Json res;
            Json& caps = res["capabilities"];
            caps["textDocumentSync"]["openClose"] = true;
            caps["textDocumentSync"]["change"] = 1;
            caps["completionProvider"]["triggerCharacters"].Push(".");
            caps["hoverProvider"] = true;
            caps["signatureHelpProvider"]["triggerCharacters"].Push("(");
            caps["signatureHelpProvider"]["triggerCharacters"].Push(",");
            caps["definitionProvider"] = true;
            caps["referencesProvider"] = true;
            caps["documentHighlightProvider"] = true;
            caps["renameProvider"]["prepareProvider"] = true;
            caps["documentSymbolProvider"] = true;
            caps["foldingRangeProvider"] = true;
            res["serverInfo"]["name"] = "litescript-pg";
#ifdef PLAYGROUND_VERSION
            res["serverInfo"]["version"] = PLAYGROUND_VERSION;
#endif
            return res;
        }

        ////// RESOLUTION //////

        // The deepest scope that contains an offset
        static std::size_t ScopeAt(const Document& doc, std::size_t offset) {
            std::size_t found = NONE;
            for (std::size_t i = 0; i < doc.analysis.scopes.size(); i++) {
                const auto& range = doc.analysis.scopes[i].range;
                // The scopes are recorded in the order of their beginning: the last one is the deepest
                if (range.start.offset <= offset && offset <= range.end.offset)
                    found = i;
            }
            return found;
        }

        // The namespace selected at an offset of the script
        static std::string NamespaceAt(const Document& doc, std::size_t offset) {
            std::string res;
            std::vector<bool> mask = MaskCommentsAndStrings(doc.text);
            std::size_t pos = 0;
            while (true) {
                pos = doc.text.find("namespace", pos);
                if (pos == std::string::npos || pos >= offset)
                    break;
                std::size_t keyword = pos;
                std::size_t i = pos + 9;
                pos = i;
                if (mask[keyword] || (keyword > 0 && IsNameChar(doc.text[keyword - 1])) || i >= doc.text.size() || !IsBlank(doc.text[i]))
                    continue;
                while (i < doc.text.size() && IsBlank(doc.text[i]))
                    i++;
                std::size_t start = i;
                while (i < doc.text.size() && (IsNameChar(doc.text[i]) || doc.text[i] == '.'))
                    i++;
                std::string path = doc.text.substr(start, i - start);
                res = path == "global" ? std::string() : path;
            }
            return res;
        }

        // The symbols declared in a scope with a name (for the script scope: in a namespace)
        static std::vector<std::size_t> DeclaredIn(const Document& doc, std::size_t scope, std::string_view name, const std::string * nspace) {
            std::vector<std::size_t> res;
            for (std::size_t i = 0; i < doc.analysis.symbols.size(); i++) {
                const auto& s = doc.analysis.symbols[i];
                if (s.scope == scope && s.name == name && s.kind != SymbolKind::Namespace && (nspace == nullptr || s.nspace == *nspace))
                    res.push_back(i);
            }
            return res;
        }

        // Find the declarations of a name visible from a scope (the first scope that declares it)
        static std::vector<std::size_t> ResolveName(const Document& doc, std::size_t scope, std::string_view name, const std::string& nspace) {
            for (std::size_t s = scope; s != NONE; s = doc.analysis.scopes[s].parent) {
                if (doc.analysis.scopes[s].kind == Analysis::Scope::Kind::Script) {
                    auto found = DeclaredIn(doc, s, name, &nspace);
                    if (found.empty() && !nspace.empty()) {
                        static const std::string global;
                        found = DeclaredIn(doc, s, name, &global);
                    }
                    if (!found.empty())
                        return found;
                    break;
                }
                auto found = DeclaredIn(doc, s, name, nullptr);
                if (!found.empty())
                    return found;
            }
            // The first part of a namespace path ("Game" for "namespace Game.Engine;")
            for (std::size_t i = 0; i < doc.analysis.symbols.size(); i++) {
                const auto& s = doc.analysis.symbols[i];
                if (s.kind == SymbolKind::Namespace && (s.name == name || s.name.rfind(std::string(name) + ".", 0) == 0))
                    return { i };
            }
            return {};
        }

        // The text of the expression before a member access ("a.b" for "a.b.c", empty if it's not a name)
        static std::string ReceiverOf(const Document& doc, std::size_t member_start) {
            std::size_t i = member_start;
            while (i > 0 && IsBlank(doc.text[i - 1]))
                i--;
            if (i == 0 || doc.text[i - 1] != '.')
                return std::string();
            i--;
            std::string res;
            while (true) {
                while (i > 0 && IsBlank(doc.text[i - 1]))
                    i--;
                std::size_t end = i;
                while (i > 0 && IsNameChar(doc.text[i - 1]))
                    i--;
                if (i == end)
                    return std::string();
                res = doc.text.substr(i, end - i) + (res.empty() ? "" : "." + res);
                std::size_t j = i;
                while (j > 0 && IsBlank(doc.text[j - 1]))
                    j--;
                if (j == 0 || doc.text[j - 1] != '.')
                    return res;
                i = j - 1;
            }
        }

        // The class scope that contains a scope (NONE if there is none)
        static std::size_t EnclosingClass(const Document& doc, std::size_t scope) {
            for (std::size_t s = scope; s != NONE; s = doc.analysis.scopes[s].parent) {
                if (doc.analysis.scopes[s].kind == Analysis::Scope::Kind::Class)
                    return s;
            }
            return NONE;
        }

        // The members with a name of the classes of the document
        static std::vector<std::size_t> AllMembers(const Document& doc, std::string_view name) {
            std::vector<std::size_t> res;
            for (std::size_t i = 0; i < doc.analysis.symbols.size(); i++) {
                const auto& s = doc.analysis.symbols[i];
                if (s.name == name && s.scope != NONE && doc.analysis.scopes[s.scope].kind == Analysis::Scope::Kind::Class)
                    res.push_back(i);
            }
            return res;
        }

        // The scope of the members designated by a receiver ("this", a class, NONE if unknown)
        // and the namespace it designates (if it's a namespace path)
        static std::size_t ReceiverScope(const Document& doc, const std::string& receiver, std::size_t scope, const std::string& nspace, std::string& namespace_path) {
            namespace_path.clear();
            if (receiver.empty())
                return NONE;
            if (receiver == "this")
                return EnclosingClass(doc, scope);
            for (const auto& s : doc.analysis.symbols) {
                if (s.kind == SymbolKind::Namespace && s.name == receiver) {
                    namespace_path = receiver;
                    return NONE;
                }
            }
            if (receiver.find('.') == std::string::npos) {
                auto found = ResolveName(doc, scope, receiver, nspace);
                if (!found.empty() && doc.analysis.symbols[found.front()].kind == SymbolKind::Class)
                    return doc.analysis.symbols[found.front()].body;
            }
            else {
                // "A.B" where A is a class: the nested class B
                std::size_t dot = receiver.rfind('.');
                std::string inner_ns;
                std::size_t outer = ReceiverScope(doc, receiver.substr(0, dot), scope, nspace, inner_ns);
                std::string name = receiver.substr(dot + 1);
                std::vector<std::size_t> found;
                if (outer != NONE)
                    found = DeclaredIn(doc, outer, name, nullptr);
                else if (!inner_ns.empty() && !doc.analysis.scopes.empty())
                    found = DeclaredIn(doc, 0, name, &inner_ns);
                if (!found.empty() && doc.analysis.symbols[found.front()].kind == SymbolKind::Class)
                    return doc.analysis.symbols[found.front()].body;
            }
            return NONE;
        }

        // The declarations designated by a reference
        static std::vector<std::size_t> Resolve(const Document& doc, const Analysis::Reference& ref) {
            if (!ref.member)
                return ResolveName(doc, ref.scope, ref.name, ref.nspace);
            std::string receiver = ReceiverOf(doc, ref.range.start.offset), path;
            std::size_t members = ReceiverScope(doc, receiver, ref.scope, ref.nspace, path);
            if (members != NONE)
                return DeclaredIn(doc, members, ref.name, nullptr);
            if (!path.empty() && !doc.analysis.scopes.empty()) {
                auto found = DeclaredIn(doc, 0, ref.name, &path);
                if (!found.empty())
                    return found;
                // A deeper namespace ("Game.Engine" from "Game")
                for (std::size_t i = 0; i < doc.analysis.symbols.size(); i++) {
                    const auto& s = doc.analysis.symbols[i];
                    std::string full = path + "." + ref.name;
                    if (s.kind == SymbolKind::Namespace && (s.name == full || s.name.rfind(full + ".", 0) == 0))
                        return { i };
                }
                return {};
            }
            // Unknown receiver: the members of every class with this name
            return receiver == "this" ? std::vector<std::size_t>() : AllMembers(doc, ref.name);
        }

        // The declaration and the uses designated by the name at an offset
        struct Target {
            // The declarations (the first one identifies the target)
            std::vector<std::size_t> symbols;
            // The range of the name at the offset
            std::size_t start = 0, end = 0;
            // The text of the name
            std::string name;
            // The receiver if it's a member access
            std::string receiver;
            bool member = false;
            bool found = false;
        };

        static Target TargetAt(const Document& doc, std::size_t offset) {
            Target target;
            for (std::size_t i = 0; i < doc.analysis.symbols.size(); i++) {
                const auto& s = doc.analysis.symbols[i];
                if (s.range.start.offset <= offset && offset <= s.range.end.offset) {
                    target.found = true;
                    target.start = s.range.start.offset;
                    target.end = s.range.end.offset;
                    target.name = s.name;
                    // The first declaration of the same name in the same scope
                    if (s.kind == SymbolKind::Namespace)
                        target.symbols = { i };
                    else
                        target.symbols = DeclaredIn(doc, s.scope, s.name, s.scope == 0 ? &s.nspace : nullptr);
                    return target;
                }
            }
            for (const auto& ref : doc.analysis.references) {
                if (ref.range.start.offset <= offset && offset <= ref.range.end.offset) {
                    target.found = true;
                    target.start = ref.range.start.offset;
                    target.end = ref.range.end.offset;
                    target.name = ref.name;
                    target.member = ref.member;
                    if (ref.member)
                        target.receiver = ReceiverOf(doc, ref.range.start.offset);
                    target.symbols = Resolve(doc, ref);
                    return target;
                }
            }
            return target;
        }

        // Every range (declarations and uses) of a target
        static std::vector<std::pair<std::size_t, std::size_t>> Occurrences(const Document& doc, const Target& target, bool declarations) {
            std::vector<std::pair<std::size_t, std::size_t>> res;
            if (target.symbols.empty())
                return res;
            std::set<std::size_t> same(target.symbols.begin(), target.symbols.end());
            // Every declaration of the same name in the same scope is the same variable
            const auto& first = doc.analysis.symbols[target.symbols.front()];
            if (first.kind != SymbolKind::Namespace) {
                for (std::size_t i : DeclaredIn(doc, first.scope, first.name, first.scope == 0 ? &first.nspace : nullptr))
                    same.insert(i);
            }
            if (declarations) {
                for (std::size_t i : same) {
                    const auto& s = doc.analysis.symbols[i];
                    res.emplace_back(s.range.start.offset, s.range.end.offset);
                }
            }
            for (const auto& ref : doc.analysis.references) {
                if (ref.name != target.name && first.kind != SymbolKind::Namespace)
                    continue;
                auto found = Resolve(doc, ref);
                if (!found.empty() && same.count(found.front()) > 0)
                    res.emplace_back(ref.range.start.offset, ref.range.end.offset);
            }
            std::sort(res.begin(), res.end());
            res.erase(std::unique(res.begin(), res.end()), res.end());
            return res;
        }

        ////// DESCRIPTIONS //////

        // The class that declares a member (empty if it's not a member)
        static std::string OwnerOf(const Document& doc, const Analysis::Symbol& symbol) {
            if (symbol.scope == NONE || doc.analysis.scopes[symbol.scope].kind != Analysis::Scope::Kind::Class)
                return std::string();
            for (const auto& s : doc.analysis.symbols) {
                if (s.body == symbol.scope)
                    return s.name;
            }
            return "(class)";
        }

        static std::string Parameters(const Analysis::Symbol& symbol) {
            std::string res;
            for (const std::string& p : symbol.parameters)
                res += (res.empty() ? "" : ", ") + p;
            return res;
        }

        static std::string Describe(const Document& doc, const Analysis::Symbol& s) {
            std::string owner = OwnerOf(doc, s);
            std::string prefix = s.is_static ? "static " : "";
            std::string qualified = owner.empty() ? s.name : owner + "." + s.name;
            switch (s.kind) {
                case SymbolKind::Variable: return "var " + s.name;
                case SymbolKind::Function: return "function " + s.name + "(" + Parameters(s) + ")";
                case SymbolKind::Class: return prefix + "class " + qualified;
                case SymbolKind::Parameter: return "(parameter) " + s.name;
                case SymbolKind::Attribute: return prefix + "var " + qualified;
                case SymbolKind::Method:
                    return (s.name == owner ? "constructor " : prefix + "function ") + qualified + "(" + Parameters(s) + ")";
                case SymbolKind::Operator: return "operator " + qualified + "(" + Parameters(s) + ")";
                case SymbolKind::CatchVariable: return "(catch) " + s.name;
                case SymbolKind::Namespace: return "namespace " + s.name;
            }
            return s.name;
        }

        static Json Markdown(const std::string& code, const std::string& text) {
            Json res;
            res["kind"] = "markdown";
            res["value"] = "```litescript\n" + code + "\n```" + (text.empty() ? "" : "\n\n" + text);
            return res;
        }

        // The API element designated by a name and the receiver of a member access
        static const ApiEntry * ApiOf(const std::string& receiver, const std::string& name, bool member) {
            if (!member)
                return Tools::FindApi(name);
            if (!receiver.empty() && receiver.find('.') == std::string::npos) {
                const ApiEntry * nsp = Tools::FindApi(receiver);
                if (nsp != nullptr && nsp->kind == ApiEntry::Kind::Namespace)
                    return Tools::FindApi(receiver + "." + name);
            }
            for (const ApiEntry& entry : Tools::GetApi()) {
                if ((entry.kind == ApiEntry::Kind::ArrayMethod || entry.kind == ApiEntry::Kind::StringMethod) && entry.name == name)
                    return &entry;
            }
            return nullptr;
        }

        ////// DIAGNOSTICS //////

        void PublishDiagnostics(const Document& doc) {
            Json diagnostics = Json::MakeArray();
            if (!doc.result) {
                // The position of the error, until the end of the name or of the line
                std::size_t start = doc.line_starts[std::min(doc.result.line, doc.line_starts.size() - 1)];
                for (std::size_t column = 0; column < doc.result.column && start < doc.text.size(); start++) {
                    if ((static_cast<unsigned char>(doc.text[start]) & 0xC0) != 0x80 && column++ == doc.result.column)
                        break;
                }
                while (start < doc.text.size() && (static_cast<unsigned char>(doc.text[start]) & 0xC0) == 0x80)
                    start++;
                std::size_t end = start;
                while (end < doc.text.size() && IsNameChar(doc.text[end]))
                    end++;
                if (end == start && end < doc.text.size() && doc.text[end] != '\n')
                    end++;
                Json d;
                d["range"] = doc.RangeOf(start, end);
                d["severity"] = Lsp::SEVERITY_ERROR;
                d["source"] = "litescript";
                d["message"] = std::string(doc.result.GetMessage());
                diagnostics.Push(std::move(d));
            }
            if (this->unknown_variables) {
                for (const auto& ref : doc.analysis.references) {
                    if (ref.member || Tools::FindApi(ref.name) != nullptr || !Resolve(doc, ref).empty())
                        continue;
                    Json d;
                    d["range"] = doc.RangeOf(ref.range);
                    d["severity"] = Lsp::SEVERITY_WARNING;
                    d["source"] = "litescript";
                    d["message"] = "\"" + ref.name + "\" isn't declared in this file nor in the libraries of the playground";
                    diagnostics.Push(std::move(d));
                }
            }
            Json params;
            params["uri"] = doc.uri;
            params["version"] = doc.version;
            params["diagnostics"] = std::move(diagnostics);
            this->Notify("textDocument/publishDiagnostics", std::move(params));
        }

        ////// COMPLETION //////

        static Json Item(const std::string& label, int kind, const std::string& detail, const std::string& documentation = "") {
            Json item;
            item["label"] = label;
            item["kind"] = kind;
            if (!detail.empty())
                item["detail"] = detail;
            if (!documentation.empty()) {
                item["documentation"]["kind"] = "markdown";
                item["documentation"]["value"] = documentation;
            }
            return item;
        }

        static int CompletionKind(SymbolKind kind, bool constructor) {
            switch (kind) {
                case SymbolKind::Function: return Lsp::COMPLETION_FUNCTION;
                case SymbolKind::Class: return Lsp::COMPLETION_CLASS;
                case SymbolKind::Attribute: return Lsp::COMPLETION_FIELD;
                case SymbolKind::Method: return constructor ? Lsp::COMPLETION_CONSTRUCTOR : Lsp::COMPLETION_METHOD;
                case SymbolKind::Operator: return Lsp::COMPLETION_OPERATOR;
                case SymbolKind::Namespace: return Lsp::COMPLETION_MODULE;
                default: return Lsp::COMPLETION_VARIABLE;
            }
        }

        static int CompletionKind(ApiEntry::Kind kind) {
            switch (kind) {
                case ApiEntry::Kind::Namespace: return Lsp::COMPLETION_MODULE;
                case ApiEntry::Kind::Value: return Lsp::COMPLETION_CONSTANT;
                case ApiEntry::Kind::ArrayMethod:
                case ApiEntry::Kind::StringMethod: return Lsp::COMPLETION_METHOD;
                default: return Lsp::COMPLETION_FUNCTION;
            }
        }

        Json Completion(const Document& doc, std::size_t offset) {
            Json items = Json::MakeArray();
            std::set<std::string> added;
            auto add = [&](Json item) {
                if (added.insert(item["label"].GetString()).second)
                    items.Push(std::move(item));
            };
            if (MaskCommentsAndStrings(doc.text)[offset])
                return items;
            // The name being typed and the member access before it
            std::size_t start = offset;
            while (start > 0 && IsNameChar(doc.text[start - 1]))
                start--;
            std::size_t scope = ScopeAt(doc, offset);
            std::string nspace = NamespaceAt(doc, offset);
            std::string receiver = ReceiverOf(doc, start);
            std::size_t before = start;
            while (before > 0 && IsBlank(doc.text[before - 1]))
                before--;
            bool member = before > 0 && doc.text[before - 1] == '.';

            if (member) {
                std::string path;
                std::size_t members = ReceiverScope(doc, receiver, scope, nspace, path);
                if (members != NONE) {
                    for (const auto& s : doc.analysis.symbols) {
                        if (s.scope == members && s.kind != SymbolKind::Operator)
                            add(Item(s.name, CompletionKind(s.kind, s.name == OwnerOf(doc, s)), Describe(doc, s)));
                    }
                    if (receiver == "this")
                        return items;
                }
                if (!path.empty()) {
                    for (const auto& s : doc.analysis.symbols) {
                        if (s.scope == 0 && s.nspace == path && s.kind != SymbolKind::Namespace)
                            add(Item(s.name, CompletionKind(s.kind, false), Describe(doc, s)));
                        // The nested namespaces
                        if (s.kind == SymbolKind::Namespace && s.name.rfind(path + ".", 0) == 0) {
                            std::string next = s.name.substr(path.size() + 1);
                            add(Item(next.substr(0, next.find('.')), Lsp::COMPLETION_MODULE, "namespace " + path + "." + next.substr(0, next.find('.'))));
                        }
                    }
                    return items;
                }
                const ApiEntry * nsp = receiver.find('.') == std::string::npos ? Tools::FindApi(receiver) : nullptr;
                if (nsp != nullptr && nsp->kind == ApiEntry::Kind::Namespace) {
                    std::string prefix = receiver + ".";
                    for (const ApiEntry& entry : Tools::GetApi()) {
                        if (entry.name.substr(0, prefix.size()) == prefix)
                            add(Item(std::string(entry.name.substr(prefix.size())), CompletionKind(entry.kind), std::string(entry.signature), std::string(entry.documentation)));
                    }
                    return items;
                }
                if (members != NONE)
                    return items;
                // Unknown value: the methods of the arrays and the strings, and the members of the classes
                for (const ApiEntry& entry : Tools::GetApi()) {
                    if (entry.kind == ApiEntry::Kind::ArrayMethod || entry.kind == ApiEntry::Kind::StringMethod)
                        add(Item(std::string(entry.name), entry.name == "length" ? Lsp::COMPLETION_PROPERTY : Lsp::COMPLETION_METHOD,
                                 std::string(entry.signature), std::string(entry.documentation)));
                }
                for (const auto& s : doc.analysis.symbols) {
                    if (s.scope != NONE && doc.analysis.scopes[s.scope].kind == Analysis::Scope::Kind::Class && s.kind != SymbolKind::Operator)
                        add(Item(s.name, CompletionKind(s.kind, false), Describe(doc, s)));
                }
                return items;
            }

            // The variables visible from the position
            for (std::size_t s = scope; s != NONE; s = doc.analysis.scopes[s].parent) {
                bool script = doc.analysis.scopes[s].kind == Analysis::Scope::Kind::Script;
                for (const auto& symbol : doc.analysis.symbols) {
                    if (symbol.scope != s || symbol.kind == SymbolKind::Namespace)
                        continue;
                    // The parameters and the variables of the functions are only visible after their declaration
                    if (script && !symbol.nspace.empty() && symbol.nspace != nspace)
                        continue;
                    add(Item(symbol.name, CompletionKind(symbol.kind, false), Describe(doc, symbol)));
                }
            }
            for (const auto& symbol : doc.analysis.symbols) {
                if (symbol.kind == SymbolKind::Namespace) {
                    std::string root = symbol.name.substr(0, symbol.name.find('.'));
                    add(Item(root, Lsp::COMPLETION_MODULE, "namespace " + root));
                }
            }
            // The names of the last code without error
            for (const std::string& name : doc.last_names) {
                if (added.count(name) == 0 && AllMembers(doc, name).empty())
                    add(Item(name, Lsp::COMPLETION_VARIABLE, ""));
            }
            for (const ApiEntry& entry : Tools::GetApi()) {
                if ((entry.kind == ApiEntry::Kind::Function || entry.kind == ApiEntry::Kind::Namespace || entry.kind == ApiEntry::Kind::Value) &&
                    entry.name.find('.') == std::string_view::npos)
                    add(Item(std::string(entry.name), CompletionKind(entry.kind), std::string(entry.signature), std::string(entry.documentation)));
            }
            for (std::string_view keyword : Tools::KEYWORDS)
                add(Item(std::string(keyword), Lsp::COMPLETION_KEYWORD, ""));
            auto snippet = [&](const std::string& label, const std::string& body, const std::string& detail) {
                Json item = Item(label, Lsp::COMPLETION_SNIPPET, detail);
                item["insertText"] = body;
                item["insertTextFormat"] = 2;
                items.Push(std::move(item));
            };
            snippet("function", "function ${1:name}(${2}) {\n\t$0\n}", "Function declaration");
            snippet("class", "class ${1:Name} {\n\tfunction ${1:Name}(${2}) {\n\t\t$0\n\t}\n}", "Class declaration");
            snippet("for", "for (var ${1:i} = 0; ${1:i} < ${2:count}; ${1:i}++) {\n\t$0\n}", "For loop");
            snippet("if", "if (${1:condition}) {\n\t$0\n}", "If statement");
            snippet("while", "while (${1:condition}) {\n\t$0\n}", "While loop");
            snippet("try", "try {\n\t$0\n}\ncatch (${1:e}) {\n\t\n}", "Try ... catch");
            snippet("switch", "switch (${1:value}) {\n\tcase ${2:1}:\n\t\t$0\n\t\tbreak;\n\tdefault:\n\t\tbreak;\n}", "Switch statement");
            return items;
        }

        ////// HOVER //////

        Json Hover(const Document& doc, std::size_t offset) {
            Target target = TargetAt(doc, offset);
            if (!target.found)
                return Json();
            Json res;
            res["range"] = doc.RangeOf(target.start, target.end);
            if (!target.symbols.empty()) {
                const auto& s = doc.analysis.symbols[target.symbols.front()];
                std::string text;
                if (!s.nspace.empty())
                    text = "In the namespace `" + s.nspace + "`";
                if (target.symbols.size() > 1 && target.member)
                    text += (text.empty() ? "" : " · ") + std::to_string(target.symbols.size()) + " classes have this member";
                res["contents"] = Markdown(Describe(doc, s), text);
                return res;
            }
            const ApiEntry * entry = ApiOf(target.receiver, target.name, target.member);
            if (entry == nullptr)
                return Json();
            std::string signature(entry->signature);
            if (entry->kind == ApiEntry::Kind::Function || entry->kind == ApiEntry::Kind::Value) {
                std::string_view name = entry->name;
                if (name.find('.') != std::string_view::npos)
                    signature = std::string(name.substr(0, name.rfind('.') + 1)) + signature;
            }
            else if (entry->kind == ApiEntry::Kind::Namespace) {
                signature = "namespace " + signature;
            }
            else {
                signature = (entry->kind == ApiEntry::Kind::StringMethod ? "(string) " : "(array) ") + signature;
            }
            res["contents"] = Markdown(signature, std::string(entry->documentation));
            return res;
        }

        ////// SIGNATURE HELP //////

        // Split "f(a, b)" into its parameters
        static std::vector<std::string> SplitParameters(std::string_view signature) {
            std::vector<std::string> res;
            std::size_t open = signature.find('('), close = signature.rfind(')');
            if (open == std::string_view::npos || close == std::string_view::npos || close <= open + 1)
                return res;
            std::string_view inside = signature.substr(open + 1, close - open - 1);
            while (true) {
                std::size_t comma = inside.find(", ");
                res.emplace_back(inside.substr(0, comma));
                if (comma == std::string_view::npos)
                    break;
                inside.remove_prefix(comma + 2);
            }
            return res;
        }

        Json SignatureHelp(const Document& doc, std::size_t offset) {
            // The calls not closed before the position
            struct Open {
                bool call;
                std::size_t paren;
                std::size_t commas;
            };
            std::vector<Open> opened;
            std::vector<bool> mask = MaskCommentsAndStrings(doc.text);
            for (std::size_t i = 0; i < offset && i < doc.text.size(); i++) {
                if (mask[i])
                    continue;
                char c = doc.text[i];
                if (c == '(' || c == '[' || c == '{')
                    opened.push_back({ c == '(', i, 0 });
                else if ((c == ')' || c == ']' || c == '}') && !opened.empty())
                    opened.pop_back();
                else if (c == ',' && !opened.empty())
                    opened.back().commas++;
            }
            if (opened.empty() || !opened.back().call)
                return Json();
            // The name of the function before the parenthesis
            std::size_t end = opened.back().paren;
            while (end > 0 && IsBlank(doc.text[end - 1]))
                end--;
            std::size_t start = end;
            while (start > 0 && IsNameChar(doc.text[start - 1]))
                start--;
            if (start == end)
                return Json();
            std::string name = doc.text.substr(start, end - start);
            std::string receiver = ReceiverOf(doc, start);
            std::size_t keyword_end = start;
            while (keyword_end > 0 && IsBlank(doc.text[keyword_end - 1]))
                keyword_end--;
            bool construction = keyword_end >= 3 && doc.text.compare(keyword_end - 3, 3, "new") == 0 &&
                                (keyword_end == 3 || !IsNameChar(doc.text[keyword_end - 4]));
            std::size_t scope = ScopeAt(doc, offset);
            std::string nspace = NamespaceAt(doc, offset);

            std::string label;
            std::vector<std::string> parameters;
            std::string documentation;
            bool member = !receiver.empty() || (start > 0 && doc.text[start - 1] == '.');
            std::vector<std::size_t> found;
            if (!member) {
                found = ResolveName(doc, scope, name, nspace);
            }
            else {
                Analysis::Reference ref;
                ref.name = name;
                ref.scope = scope;
                ref.member = true;
                ref.nspace = nspace;
                ref.range.start.offset = start;
                found = Resolve(doc, ref);
            }
            if (!found.empty()) {
                const auto& s = doc.analysis.symbols[found.front()];
                if (s.kind == SymbolKind::Class && s.body != NONE) {
                    // The constructor of the class
                    for (const auto& m : doc.analysis.symbols) {
                        if (m.scope == s.body && m.kind == SymbolKind::Method && (m.name == s.name || m.name == "constructor")) {
                            parameters = m.parameters;
                            break;
                        }
                    }
                    label = (construction ? "new " : "") + s.name;
                }
                else if (!s.parameters.empty() || s.kind == SymbolKind::Function || s.kind == SymbolKind::Method) {
                    parameters = s.parameters;
                    label = s.name;
                }
                else {
                    return Json();
                }
            }
            else if (const ApiEntry * entry = ApiOf(receiver, name, member)) {
                parameters = SplitParameters(entry->signature);
                label = (entry->name.find('.') != std::string_view::npos ? receiver + "." : std::string()) + name;
                documentation = entry->documentation;
            }
            else {
                return Json();
            }

            label += "(";
            Json params = Json::MakeArray();
            for (std::size_t i = 0; i < parameters.size(); i++) {
                if (i > 0)
                    label += ", ";
                Json p;
                p["label"].Push(Json(label.size()));
                label += parameters[i];
                p["label"].Push(Json(label.size()));
                params.Push(std::move(p));
            }
            label += ")";
            Json signature;
            signature["label"] = label;
            signature["parameters"] = std::move(params);
            if (!documentation.empty()) {
                signature["documentation"]["kind"] = "markdown";
                signature["documentation"]["value"] = documentation;
            }
            std::size_t active = opened.back().commas;
            if (!parameters.empty() && active >= parameters.size())
                active = parameters.size() - 1;
            Json res;
            res["signatures"].Push(std::move(signature));
            res["activeSignature"] = 0;
            res["activeParameter"] = active;
            return res;
        }

        ////// NAVIGATION //////

        Json Definition(const Document& doc, std::size_t offset) {
            Target target = TargetAt(doc, offset);
            Json res = Json::MakeArray();
            for (std::size_t i : target.symbols)
                res.Push(doc.LocationOf(doc.analysis.symbols[i].range));
            return res;
        }

        Json References(const Document& doc, std::size_t offset, bool declarations) {
            Json res = Json::MakeArray();
            for (const auto& [start, end] : Occurrences(doc, TargetAt(doc, offset), declarations)) {
                Json location;
                location["uri"] = doc.uri;
                location["range"] = doc.RangeOf(start, end);
                res.Push(std::move(location));
            }
            return res;
        }

        Json Highlights(const Document& doc, std::size_t offset) {
            Json res = Json::MakeArray();
            for (const auto& [start, end] : Occurrences(doc, TargetAt(doc, offset), true)) {
                Json highlight;
                highlight["range"] = doc.RangeOf(start, end);
                res.Push(std::move(highlight));
            }
            return res;
        }

        Json PrepareRename(const Document& doc, std::size_t offset) {
            Target target = TargetAt(doc, offset);
            if (target.symbols.empty() || doc.analysis.symbols[target.symbols.front()].kind == SymbolKind::Namespace)
                throw std::pair<int, std::string>(Lsp::ERROR_REQUEST_FAILED, "Only the declarations of this file can be renamed");
            Json res;
            res["range"] = doc.RangeOf(target.start, target.end);
            res["placeholder"] = target.name;
            return res;
        }

        Json Rename(const Document& doc, std::size_t offset, const std::string& name) {
            Target target = TargetAt(doc, offset);
            if (target.symbols.empty() || doc.analysis.symbols[target.symbols.front()].kind == SymbolKind::Namespace)
                throw std::pair<int, std::string>(Lsp::ERROR_REQUEST_FAILED, "Only the declarations of this file can be renamed");
            bool valid = !name.empty() && !(name[0] >= '0' && name[0] <= '9') &&
                         std::all_of(name.begin(), name.end(), IsNameChar) &&
                         std::find(Tools::KEYWORDS.begin(), Tools::KEYWORDS.end(), name) == Tools::KEYWORDS.end();
            if (!valid)
                throw std::pair<int, std::string>(Lsp::ERROR_REQUEST_FAILED, "\"" + name + "\" isn't a valid name");
            Json edits = Json::MakeArray();
            for (const auto& [start, end] : Occurrences(doc, target, true)) {
                Json edit;
                edit["range"] = doc.RangeOf(start, end);
                edit["newText"] = name;
                edits.Push(std::move(edit));
            }
            Json res;
            res["changes"][doc.uri] = std::move(edits);
            return res;
        }

        ////// STRUCTURE //////

        Json SymbolOf(const Document& doc, std::size_t index) {
            const auto& s = doc.analysis.symbols[index];
            std::string owner = OwnerOf(doc, s);
            int kind = Lsp::SYMBOL_VARIABLE;
            switch (s.kind) {
                case SymbolKind::Function: kind = Lsp::SYMBOL_FUNCTION; break;
                case SymbolKind::Class: kind = Lsp::SYMBOL_CLASS; break;
                case SymbolKind::Attribute: kind = Lsp::SYMBOL_FIELD; break;
                case SymbolKind::Method: kind = s.name == owner ? Lsp::SYMBOL_CONSTRUCTOR : Lsp::SYMBOL_METHOD; break;
                case SymbolKind::Operator: kind = Lsp::SYMBOL_OPERATOR; break;
                case SymbolKind::Namespace: kind = Lsp::SYMBOL_NAMESPACE; break;
                default: break;
            }
            Json res;
            res["name"] = s.kind == SymbolKind::Operator ? "operator " + s.name : s.name;
            if (!s.parameters.empty() || s.kind == SymbolKind::Function || s.kind == SymbolKind::Method || s.kind == SymbolKind::Operator)
                res["detail"] = "(" + Parameters(s) + ")";
            else if (s.is_static)
                res["detail"] = "static";
            res["kind"] = kind;
            res["range"] = doc.RangeOf(s.full);
            res["selectionRange"] = doc.RangeOf(s.range);
            // The members of a class, the functions and the classes declared in a function
            Json children = Json::MakeArray();
            if (s.body != NONE) {
                bool is_class = s.kind == SymbolKind::Class;
                for (std::size_t i = 0; i < doc.analysis.symbols.size(); i++) {
                    const auto& child = doc.analysis.symbols[i];
                    if (child.scope == s.body && (is_class || child.kind == SymbolKind::Function || child.kind == SymbolKind::Class))
                        children.Push(this->SymbolOf(doc, i));
                }
            }
            res["children"] = std::move(children);
            return res;
        }

        Json DocumentSymbols(const Document& doc) {
            Json res = Json::MakeArray();
            if (doc.analysis.scopes.empty())
                return res;
            // The declarations of a namespace are the children of its first "namespace" instruction
            std::map<std::string, std::size_t> namespaces;
            for (std::size_t i = 0; i < doc.analysis.symbols.size(); i++) {
                const auto& s = doc.analysis.symbols[i];
                if (s.scope != 0 || s.kind == SymbolKind::Parameter)
                    continue;
                if (s.kind == SymbolKind::Namespace) {
                    if (namespaces.count(s.name) == 0) {
                        namespaces[s.name] = res.Size();
                        Json item = this->SymbolOf(doc, i);
                        res.Push(std::move(item));
                    }
                    continue;
                }
                Json item = this->SymbolOf(doc, i);
                auto it = namespaces.find(s.nspace);
                if (!s.nspace.empty() && it != namespaces.end()) {
                    // Rebuild the namespace with the new child (the items are values)
                    Json updated = Json::MakeArray();
                    for (std::size_t j = 0; j < res.Size(); j++) {
                        Json copy = res.At(j);
                        if (j == it->second) {
                            copy["children"].Push(std::move(item));
                            // The range of the namespace covers its declarations
                            copy["range"]["end"] = doc.PositionOf(s.full.end.offset);
                        }
                        updated.Push(std::move(copy));
                    }
                    res = std::move(updated);
                }
                else {
                    res.Push(std::move(item));
                }
            }
            return res;
        }

        Json FoldingRanges(const Document& doc) {
            Json res = Json::MakeArray();
            auto add = [&](std::size_t start, std::size_t end, const char * kind) {
                Json start_pos = doc.PositionOf(start), end_pos = doc.PositionOf(end);
                if (end_pos["line"].GetInt() <= start_pos["line"].GetInt())
                    return;
                Json range;
                range["startLine"] = start_pos["line"];
                range["endLine"] = Json(end_pos["line"].GetInt() - (kind == nullptr ? 1 : 0));
                if (kind != nullptr)
                    range["kind"] = kind;
                res.Push(std::move(range));
            };
            for (const auto& s : doc.analysis.symbols) {
                if (s.body != NONE)
                    add(s.full.start.offset, s.full.end.offset, nullptr);
            }
            // The block comments (the character before the comment is code)
            std::vector<bool> mask = MaskCommentsAndStrings(doc.text);
            std::size_t pos = 0;
            while ((pos = doc.text.find("/*", pos)) != std::string::npos) {
                if (pos > 0 && mask[pos - 1]) {
                    pos += 2;
                    continue;
                }
                std::size_t end = doc.text.find("*/", pos + 2);
                if (end == std::string::npos)
                    break;
                add(pos, end, "comment");
                pos = end + 2;
            }
            return res;
        }

    };

}

int Tools::RunLanguageServer() {
    SetBinaryStandardStreams();
    Transport transport(std::cin.rdbuf(), std::cout.rdbuf());
    LanguageServer server(transport);
    return server.Run();
}
