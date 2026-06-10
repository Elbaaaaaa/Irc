/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands_connection.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:47:57 by ntamacha          #+#    #+#             */
/*   Updated: 2026/06/09 11:47:57 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <cstddef>

void Server::CAP(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;

    if (message.params.empty())
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " CAP :Not enough parameters");
        return;
    }

    std::string s = message.params[0];

    if (s == "LS")
    {
        sendToClient(fd, "CAP * LS :");
    }
    else if (s == "REQ")
    {
        sendToClient(fd, "CAP * NAK :");
    }
    else if (s == "END")
    {}
    else
    {}
}

void Server::PASS(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;
    if (client->isRegistered())
    {
        sendToClient(fd, ERR_ALREADYREGISTRED(client->getNick()));
        return ;
    }

    if (message.params.empty())
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " PASS :Not enough parameters");
        return;
    }
    std::string mdp = message.params[0];

    _clientPassword[fd] = mdp;

}

int isValidNick(const std::string& nick)
{
    size_t i = 0;
    std::string allowed = "-[]{}|\\";
    if (nick[i] != '_' && !isalpha(nick[i]))
        return (0);
    for (i = 1; i < nick.size(); i++)
    {
        if (allowed.find(nick[i]) == std::string::npos && !isalpha(nick[i]) && !isdigit(nick[i]))
            return 0;
    }
    return 1;   

}

void Server::NICK(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;
     if (message.params.empty())
    {
        sendToClient(fd, ERR_NONICKNAMEGIVEN(client->getNick()));
        return;
    }
    std::string nick = message.params[0];

    if (!isValidNick(nick))
    {
        sendToClient(fd, ERR_ERRONEUSNICKNAME(client->getNick(), nick));
        return;
    }

    Client* c = getClientByNick(nick);
    if (c)
    {
        sendToClient(fd, ERR_NICKNAMEINUSE(client->getNick(), nick));
        return ;
    }

    client->setNick(nick);

    if (!client->getUsername().empty() && !client->getHostname().empty() && _clientPassword[fd] == _password)
    {
        std::string welcome = ":server 001 " + client->getNick() + " :Welcome to the IRC server " + client->getPrefix();
        sendToClient(fd, welcome);
        client->setRegistered(true);
    }
}

void Server::USER(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;
    if (client->isRegistered())
    {
        sendToClient (fd, ERR_ALREADYREGISTRED(client->getNick()));
        return ;
    }
    if (message.params.size() < 4)
    {
        sendToClient(fd, ERR_NEEDMOREPARAMS(client->getNick(), "USER"));
        return;
    }
    client->setUsername(message.params[0]);
    client->setRealname(message.params[3]);
    if (!client->getUsername().empty() && !client->getHostname().empty() && _clientPassword[fd] == _password)
    {
        std::string welcome = ":server 001 " + client->getNick() + " :Welcome to the IRC server " + client->getPrefix();
        sendToClient(fd, welcome);
        client->setRegistered(true);
    }

}

void Server::QUIT(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;
    std::string msg = (message.params.empty()) ? "leave" : message.params[0];

    std::string quitMsg = ":" + client->getPrefix() + " QUIT :" + msg;
    std::map<std::string, Channel*>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
        Channel* channel = getChannel(it->first);
        if (channel->CheckMember(client->getFd()))
            channel->broadcast(quitMsg);
    }
    disconnectClient(fd);
}

