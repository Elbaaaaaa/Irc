#include "../includes/Parser.hpp"
#include <sstream>
#include <algorithm>

// Helper to trim end CR/LF and spaces
static std::string trim_crlf(const std::string& s) {
    std::string res = s;
    // remove trailing \r and \n (C++98 compatible)
    while (!res.empty()) {
        char c = res[res.size() - 1];
        if (c == '\n' || c == '\r')
            res.erase(res.size() - 1, 1);
        else
            break;
    }
    return res;
}

IrcMessage Parser::parseMessage(const std::string& raw) {
    IrcMessage msg;
    std::string line = trim_crlf(raw);

    // If empty after trimming, return empty message
    if (line.empty())
        return msg;

    size_t pos = 0;

    // optional prefix
    if (line[pos] == ':') {
        ++pos;
        size_t space = line.find(' ', pos);
        if (space == std::string::npos) {
            msg.prefix = line.substr(pos);
            return msg;
        }
        msg.prefix = line.substr(pos, space - pos);
        pos = space + 1;
    }

    // skip any extra spaces
    while (pos < line.size() && line[pos] == ' ') ++pos;

    // command
    size_t next = line.find(' ', pos);
    if (next == std::string::npos) {
        msg.command = line.substr(pos);
        return msg;
    }
    msg.command = line.substr(pos, next - pos);
    pos = next + 1;

    // params
    while (pos < line.size()) {
        if (line[pos] == ' ') { ++pos; continue; }
        if (line[pos] == ':') {
            // trailing param: take rest of the line (without leading ':')
            msg.params.push_back(line.substr(pos + 1));
            break;
        }
        size_t sp = line.find(' ', pos);
        if (sp == std::string::npos) {
            msg.params.push_back(line.substr(pos));
            break;
        }
        msg.params.push_back(line.substr(pos, sp - pos));
        pos = sp + 1;
    }

    return msg;
}
