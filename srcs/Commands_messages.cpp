/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands_messages.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 10:30:30 by ntamacha          #+#    #+#             */
/*   Updated: 2026/06/10 10:30:30 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void Server::PING(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;

    if (message.params.empty())
        return ;

    sendToClient(fd, "PONG :" + message.params[0]);
}

void Server::PRIVMSG(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;
    if (message.params.size() < 2)
    {
        sendToClient(fd, ERR_NEEDMOREPARAMS(client->getNick(), "PRIVMSG"));
        return;
    }

    std::string target = message.params[0];
    std::string msg = message.params[1];
    std::string privmsg = ":" + client->getPrefix() + " PRIVMSG " + target + " :" + msg + "\r\n";

    if (message.params[0][0] != '#')
    {
        Client* cible = getClientByNick(target);
        if (!cible)
        {
           sendToClient(fd, ERR_NOSUCHNICK(client->getNick(), target));
           return ; 
        }
        sendToClient(cible->getFd(), privmsg);
    }
    else
    {
        Channel* channel = getChannel(target);
        if (!channel)
        {
            sendToClient(fd, ":server 403 " + client->getNick() + " " + target + " :No such channel");
            return;
        }
        if (!channel->CheckMember(fd))
        {
            sendToClient(fd, ERR_NOTONCHANNEL(client->getNick(), target));
            return;
        }
        channel->broadcast(privmsg, fd);
    }
}


void Server::NOTICE(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;
    if (message.params.size() < 2)
        return;


    std::string target = message.params[0];
    std::string msg = message.params[1];
    std::string privmsg = ":" + client->getPrefix() + " NOTICE " + target + " :" + msg + "\r\n" ;

    if (message.params[0][0] != '#')
    {
        Client* cible = getClientByNick(target);
        if (!cible)
           return ; 

        sendToClient(cible->getFd(), privmsg);
    }
    else
    {
        Channel* channel = getChannel(target);
        if (!channel)
            return;

        if (!channel->CheckMember(fd))
            return;

        channel->broadcast(privmsg, fd);
    }
}