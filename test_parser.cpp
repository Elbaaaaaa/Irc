#include <iostream>
#include "includes/Parser.hpp"

static void printMsg(const IrcMessage& m) {
    std::cout << "prefix: '" << m.prefix << "'\n";
    std::cout << "command: '" << m.command << "'\n";
    std::cout << "params:\n";
    for (size_t i = 0; i < m.params.size(); ++i)
        std::cout << "  [" << i << "] '" << m.params[i] << "'\n";
}

int main() {
    {
        std::string s = ":nick!user@host PRIVMSG #chan :Hello there\r\n";
        IrcMessage m = Parser::parseMessage(s);
        std::cout << "Test 1\n";
        printMsg(m);
    }
    {
        std::string s = "PING :12345\r\n";
        IrcMessage m = Parser::parseMessage(s);
        std::cout << "Test 2\n";
        printMsg(m);
    }
    {
        std::string s = "NICK newnick\r\n";
        IrcMessage m = Parser::parseMessage(s);
        std::cout << "Test 3\n";
        printMsg(m);
    }
    return 0;
}
