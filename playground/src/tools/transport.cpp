#include "transport.hpp"

#include <cstdio>
#include <string>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

std::optional<Tools::Json> Tools::Transport::Read() {
    std::size_t length = 0;
    bool has_length = false;
    std::string line;
    while (true) {
        if (!std::getline(this->in, line))
            return std::nullopt;
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.empty()) {
            if (has_length)
                break;
            continue;
        }
        constexpr std::string_view header = "Content-Length:";
        if (line.size() > header.size() && line.compare(0, header.size(), header) == 0) {
            length = static_cast<std::size_t>(std::stoull(line.substr(header.size())));
            has_length = true;
        }
    }
    std::string content(length, '\0');
    if (!this->in.read(content.data(), static_cast<std::streamsize>(length)))
        return std::nullopt;
    auto message = Json::Parse(content);
    return message ? std::move(*message) : Json::MakeObject();
}

void Tools::Transport::Write(const Json& message) {
    std::string content = message.Dump();
    std::lock_guard<std::mutex> lock(this->write_mutex);
    this->out << "Content-Length: " << content.size() << "\r\n\r\n" << content;
    this->out.flush();
}

void Tools::SetBinaryStandardStreams() {
#ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}
