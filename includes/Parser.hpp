#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "IrcMessage.hpp"

class Parser {
public:
    // Parse a raw IRC line (may contain trailing CR/LF). Returns an IrcMessage.
    // Conforms to: [:prefix] COMMAND [params] [:trailing]\r\n
    static IrcMessage parseMessage(const std::string& raw);
};

#endif // PARSER_HPP
