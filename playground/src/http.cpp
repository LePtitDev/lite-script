#include "helpers.hpp"
#include "modules.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <memory>

// The HTTP client is chosen by the build: WinHTTP on Windows, libcurl elsewhere (if found)
#if defined(PLAYGROUND_HTTP_WINHTTP)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winhttp.h>
#elif defined(PLAYGROUND_HTTP_CURL)
#include <curl/curl.h>
#elif defined(_MSC_VER)
// Without HTTP client the requests always throw, the code after them is unreachable
#pragma warning(disable: 4702)
#endif

namespace {

    using namespace Playground;

    // A request built from the options of the script
    struct Request {
        std::string method = "GET";
        std::string url;
        std::vector<std::pair<std::string, std::string>> headers;
        std::string body;
        // Timeout in milliseconds
        long timeout = 30000;
        bool follow_redirects = true;
    };

    // The response given to the script
    struct Response {
        long status = 0;
        // Header names in lower case (the values of repeated headers are joined by ", ")
        std::map<std::string, std::string> headers;
        std::string body;
    };

    std::string ToLower(std::string text) {
        for (char& c : text)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return text;
    }

    [[maybe_unused]] std::string Trim(std::string_view text) {
        while (!text.empty() && (text.front() == ' ' || text.front() == '\t'))
            text.remove_prefix(1);
        while (!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\r' || text.back() == '\n'))
            text.remove_suffix(1);
        return std::string(text);
    }

    // Add a header line "Name: value" to a response (other lines are ignored)
    [[maybe_unused]] void AddHeaderLine(Response& response, std::string_view line) {
        std::size_t colon = line.find(':');
        if (colon == std::string_view::npos)
            return;
        std::string name = ToLower(Trim(line.substr(0, colon)));
        std::string value = Trim(line.substr(colon + 1));
        auto [it, inserted] = response.headers.try_emplace(name, value);
        if (!inserted)
            it->second += ", " + value;
    }

    bool HasHeader(const Request& request, std::string_view name) {
        return std::any_of(request.headers.begin(), request.headers.end(), [&](const auto& h) { return ToLower(h.first) == name; });
    }

#if defined(PLAYGROUND_HTTP_WINHTTP)

    constexpr bool AVAILABLE = true;

    std::wstring Widen(std::string_view utf8) {
        if (utf8.empty())
            return std::wstring();
        int size = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
        std::wstring result(static_cast<std::size_t>(size), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), result.data(), size);
        return result;
    }

    std::string Narrow(std::wstring_view text) {
        if (text.empty())
            return std::string();
        int size = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
        std::string result(static_cast<std::size_t>(size), '\0');
        WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), size, nullptr, nullptr);
        return result;
    }

    // Close a WinHTTP handle at the end of the scope
    struct Handle {
        HINTERNET h = nullptr;
        explicit Handle(HINTERNET handle) : h(handle) {}
        Handle(const Handle&) = delete;
        Handle& operator=(const Handle&) = delete;
        ~Handle() { if (this->h != nullptr) WinHttpCloseHandle(this->h); }
        explicit operator bool() const noexcept { return this->h != nullptr; }
    };

    [[noreturn]] void Fail(std::string_view what) {
        DWORD code = GetLastError();
        std::string message;
        wchar_t * buffer = nullptr;
        DWORD length = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            GetModuleHandleW(L"winhttp.dll"), code, 0, reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);
        if (length > 0 && buffer != nullptr)
            message = Trim(Narrow(std::wstring_view(buffer, length)));
        if (buffer != nullptr)
            LocalFree(buffer);
        if (message.empty())
            message = "error " + std::to_string(code);
        throw std::runtime_error("HTTP request failed (" + std::string(what) + "): " + message);
    }

    Response Send(const Request& request) {
        std::wstring url = Widen(request.url);
        URL_COMPONENTS parts{};
        parts.dwStructSize = sizeof(parts);
        parts.dwHostNameLength = static_cast<DWORD>(-1);
        parts.dwUrlPathLength = static_cast<DWORD>(-1);
        parts.dwExtraInfoLength = static_cast<DWORD>(-1);
        if (!WinHttpCrackUrl(url.c_str(), static_cast<DWORD>(url.size()), 0, &parts))
            throw std::invalid_argument("invalid URL \"" + request.url + "\"");
        if (parts.nScheme != INTERNET_SCHEME_HTTP && parts.nScheme != INTERNET_SCHEME_HTTPS)
            throw std::invalid_argument("unsupported URL scheme \"" + request.url + "\"");
        std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
        std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength);
        path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
        if (path.empty())
            path = L"/";

#ifdef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
        Handle session(WinHttpOpen(L"LiteScript-Playground/2.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
#else
        Handle session(WinHttpOpen(L"LiteScript-Playground/2.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
#endif
        if (!session)
            Fail("session");
        int timeout = static_cast<int>(request.timeout);
        WinHttpSetTimeouts(session.h, timeout, timeout, timeout, timeout);
        Handle connection(WinHttpConnect(session.h, host.c_str(), parts.nPort, 0));
        if (!connection)
            Fail("connection");
        std::wstring method = Widen(request.method);
        DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
        Handle handle(WinHttpOpenRequest(connection.h, method.c_str(), path.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags));
        if (!handle)
            Fail("request");
        if (!request.follow_redirects) {
            DWORD policy = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
            WinHttpSetOption(handle.h, WINHTTP_OPTION_REDIRECT_POLICY, &policy, sizeof(policy));
        }
        // Decompress gzip and deflate responses (Windows 8.1 and newer)
#ifdef WINHTTP_OPTION_DECOMPRESSION
        DWORD decompression = WINHTTP_DECOMPRESSION_FLAG_ALL;
        WinHttpSetOption(handle.h, WINHTTP_OPTION_DECOMPRESSION, &decompression, sizeof(decompression));
#endif
        for (const auto& [name, value] : request.headers) {
            std::wstring line = Widen(name + ": " + value);
            if (!WinHttpAddRequestHeaders(handle.h, line.c_str(), static_cast<DWORD>(line.size()), WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE))
                Fail("header \"" + name + "\"");
        }
        auto size = static_cast<DWORD>(request.body.size());
        void * body = request.body.empty() ? WINHTTP_NO_REQUEST_DATA : const_cast<char *>(request.body.data());
        if (!WinHttpSendRequest(handle.h, WINHTTP_NO_ADDITIONAL_HEADERS, 0, body, size, size, 0))
            Fail("send");
        if (!WinHttpReceiveResponse(handle.h, nullptr))
            Fail("receive");

        Response response;
        DWORD status = 0, length = sizeof(status);
        if (!WinHttpQueryHeaders(handle.h, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &length, WINHTTP_NO_HEADER_INDEX))
            Fail("status");
        response.status = static_cast<long>(status);

        length = 0;
        WinHttpQueryHeaders(handle.h, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_OUTPUT_BUFFER, &length, WINHTTP_NO_HEADER_INDEX);
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            std::wstring raw(length / sizeof(wchar_t), L'\0');
            if (WinHttpQueryHeaders(handle.h, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, raw.data(), &length, WINHTTP_NO_HEADER_INDEX)) {
                std::string headers = Narrow(std::wstring_view(raw.data(), length / sizeof(wchar_t)));
                std::size_t start = headers.find("\r\n");
                while (start != std::string::npos && start + 2 < headers.size()) {
                    std::size_t end = headers.find("\r\n", start + 2);
                    AddHeaderLine(response, std::string_view(headers).substr(start + 2, end == std::string::npos ? std::string::npos : end - start - 2));
                    start = end;
                }
            }
        }

        while (true) {
            DWORD available = 0;
            if (!WinHttpQueryDataAvailable(handle.h, &available))
                Fail("read");
            if (available == 0)
                break;
            std::size_t offset = response.body.size();
            response.body.resize(offset + available);
            DWORD read = 0;
            if (!WinHttpReadData(handle.h, response.body.data() + offset, available, &read))
                Fail("read");
            response.body.resize(offset + read);
        }
        return response;
    }

#elif defined(PLAYGROUND_HTTP_CURL)

    constexpr bool AVAILABLE = true;

    std::size_t WriteBody(char * data, std::size_t size, std::size_t count, void * user) {
        static_cast<Response *>(user)->body.append(data, size * count);
        return size * count;
    }

    std::size_t WriteHeader(char * data, std::size_t size, std::size_t count, void * user) {
        auto * response = static_cast<Response *>(user);
        std::string_view line(data, size * count);
        // A new response starts (redirection): only the headers of the last one are kept
        if (line.starts_with("HTTP/"))
            response->headers.clear();
        else
            AddHeaderLine(*response, line);
        return size * count;
    }

    Response Send(const Request& request) {
        static const bool initialized = curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK;
        if (!initialized)
            throw std::runtime_error("HTTP request failed: libcurl can't be initialized");
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), &curl_easy_cleanup);
        if (!curl)
            throw std::runtime_error("HTTP request failed: libcurl can't be initialized");
        std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers(nullptr, &curl_slist_free_all);
        for (const auto& [name, value] : request.headers)
            headers.reset(curl_slist_append(headers.release(), (name + ": " + value).c_str()));
        Response response;
        char error[CURL_ERROR_SIZE] = {};
        CURL * h = curl.get();
        curl_easy_setopt(h, CURLOPT_URL, request.url.c_str());
        if (request.method == "HEAD")
            curl_easy_setopt(h, CURLOPT_NOBODY, 1L);
        else if (request.method != "GET" && request.method != "POST")
            curl_easy_setopt(h, CURLOPT_CUSTOMREQUEST, request.method.c_str());
        if (request.method == "POST" || !request.body.empty()) {
            curl_easy_setopt(h, CURLOPT_POSTFIELDS, request.body.c_str());
            curl_easy_setopt(h, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(request.body.size()));
        }
        curl_easy_setopt(h, CURLOPT_HTTPHEADER, headers.get());
        curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, request.follow_redirects ? 1L : 0L);
        curl_easy_setopt(h, CURLOPT_TIMEOUT_MS, request.timeout);
        curl_easy_setopt(h, CURLOPT_USERAGENT, "LiteScript-Playground/2.0");
        curl_easy_setopt(h, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(h, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(h, CURLOPT_ERRORBUFFER, error);
        curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, WriteBody);
        curl_easy_setopt(h, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(h, CURLOPT_HEADERFUNCTION, WriteHeader);
        curl_easy_setopt(h, CURLOPT_HEADERDATA, &response);
        CURLcode code = curl_easy_perform(h);
        if (code != CURLE_OK)
            throw std::runtime_error("HTTP request failed: " + std::string(error[0] != '\0' ? error : curl_easy_strerror(code)));
        curl_easy_getinfo(h, CURLINFO_RESPONSE_CODE, &response.status);
        return response;
    }

#else

    constexpr bool AVAILABLE = false;

#endif

    // Read a member of the options (undefined if absent)
    Variable Option(State& state, const Variable& options, std::string_view name) {
        return GetMember(state.GetMemory(), options, name);
    }

    // Read the headers given as an array with named elements
    void ReadHeaders(Request& request, const Variable& headers) {
        if (headers.Is<Undefined>() || headers.Is<Null>())
            return;
        if (!headers.Is<Array>())
            throw std::invalid_argument("the headers must be an array with named elements");
        for (const auto& [name, value] : headers.As<Array>().named) {
            if (!value.Is<Undefined>() && !value.Is<Null>())
                request.headers.emplace_back(name, ToText(value).ToUTF8());
        }
    }

    // The body is sent as it is if it's a string, other values are sent as JSON
    void ReadBody(Request& request, const Variable& body) {
        if (body.Is<Undefined>() || body.Is<Null>())
            return;
        if (body.Is<LiteScript::String>() || body.Is<Character>()) {
            request.body = ToText(body).ToUTF8();
            return;
        }
        auto json = Stringify(body);
        if (!json)
            throw std::invalid_argument("the body can't be converted into JSON");
        request.body = std::move(*json);
        if (!HasHeader(request, "content-type"))
            request.headers.emplace_back("Content-Type", "application/json");
    }

    // Send a request and convert the response into an array:
    // { status, ok, headers: { name: value }, body }
    Variable Perform([[maybe_unused]] State& state, [[maybe_unused]] const Request& request) {
#if !defined(PLAYGROUND_HTTP_WINHTTP) && !defined(PLAYGROUND_HTTP_CURL)
        throw std::runtime_error("HTTP isn't available: the playground has been built without HTTP client (libcurl not found)");
#else
        Response response = Send(request);
        Memory& memory = state.GetMemory();
        Array headers;
        for (const auto& [name, value] : response.headers)
            headers.named.Set(name, memory.CreateString(value));
        Array result;
        result.named.Set("status", memory.CreateNumber(static_cast<std::int64_t>(response.status)));
        result.named.Set("ok", memory.CreateBoolean(response.status >= 200 && response.status < 300));
        result.named.Set("headers", memory.Create(std::move(headers)));
        result.named.Set("body", memory.CreateString(response.body));
        return memory.Create(std::move(result));
#endif
    }

    // Create a function that sends a request with a fixed method: f(url, headers) or f(url, body, headers)
    NativeFunction::Signature Shortcut(std::string method, bool with_body) {
        return [method, with_body](State& s, Args& args) {
            Request request;
            request.method = method;
            request.url = TextArg(args, 0, "url");
            std::size_t headers = 1;
            if (with_body) {
                ReadBody(request, Arg(s, args, 1));
                headers = 2;
            }
            ReadHeaders(request, Arg(s, args, headers));
            return Perform(s, request);
        };
    }

}

void Playground::InstallHttp(State& state) {
    Memory& memory = state.GetMemory();
    Variable http = memory.CreateNamespace();
    // request(options) with options = { url, method, headers, body, timeout, followRedirects },
    // request(url) sends a GET request
    Define(state, http, "Http.", "request", [](State& s, Args& args) {
        Variable options = Arg(s, args, 0);
        Request request;
        if (options.Is<Array>()) {
            request.url = TextArg({ Option(s, options, "url") }, 0, "url");
            Variable method = Option(s, options, "method");
            if (!method.Is<Undefined>() && !method.Is<Null>()) {
                request.method = ToText(method).ToUTF8();
                std::transform(request.method.begin(), request.method.end(), request.method.begin(),
                    [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            }
            ReadHeaders(request, Option(s, options, "headers"));
            ReadBody(request, Option(s, options, "body"));
            Variable timeout = Option(s, options, "timeout");
            if (timeout.Is<Number>())
                request.timeout = static_cast<long>(std::clamp<std::int64_t>(timeout.As<Number>().ToInteger(), 1, 3600000));
            Variable follow = Option(s, options, "followRedirects");
            if (!follow.Is<Undefined>() && !follow.Is<Null>())
                request.follow_redirects = IsTruthy(follow);
        }
        else {
            request.url = TextArg(args, 0, "url");
        }
        return Perform(s, request);
    });
    Define(state, http, "Http.", "get", Shortcut("GET", false));
    Define(state, http, "Http.", "delete", Shortcut("DELETE", false));
    Define(state, http, "Http.", "post", Shortcut("POST", true));
    Define(state, http, "Http.", "put", Shortcut("PUT", true));
    Define(state, http, "Http.", "patch", Shortcut("PATCH", true));
    http.As<Namespace>().variables.Set("available", memory.CreateBoolean(AVAILABLE));
    state.DeclareVariable("Http", http);
}
