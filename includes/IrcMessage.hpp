// Simple IRC message representation
#ifndef IRC_MESSAGE_HPP
#define IRC_MESSAGE_HPP

#include <string>
#include <vector>

struct IrcMessage {
    std::string prefix;    // optional
    std::string command;   // command or numeric reply
    std::vector<std::string> params; // parameters, last may contain spaces (trailing)
};

#endif // IRC_MESSAGE_HPP
