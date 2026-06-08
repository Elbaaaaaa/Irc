/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands_mode.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 20:07:37 by ntamacha          #+#    #+#             */
/*   Updated: 2026/05/19 20:07:37 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"

void Server::MODE(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;
    if(message.params.size() < 2)
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " MODE :Not enough parameters\r\n");
        return;
    }
    std::string channelName = message.params[0];
    char modeChar = message.params[1][1];
    char activeChar = message.params[1][0];
    if (channelName[0] != '#')
    {
        sendToClient(fd, ":server 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    std::string key = (message.params.size() >= 3) ? message.params[2] : "";
    Channel* channel = getChannel(channelName);
    if (!channel)
    {
        sendToClient(fd, ":server 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channel->CheckMember(fd))
    {
        sendToClient(fd, ERR_NOTONCHANNEL(client->getNick(), channelName));
        return;
    }
    if (!channel->CheckOp(fd))
    {
        sendToClient(fd, ERR_CHANOPRIVSNEEDED(client->getNick(), channelName));
        return;
    }

    int active;
    switch (activeChar)
{
    case '+': active = 1;
        break;
    case '-': active = -1;
        break;
    
    default:
        sendToClient(fd, ERR_UNKNOWNMODE(client->getNick(), channelName));
        return;
}

    switch (modeChar)
{
    case 'i':
        if (active == 1)
            channel->SetModei(true);
        else
            channel->SetModei(false); 
        std::string modeMsg = ":" + client->getPrefix() + " MODE " + channelName + " " + message.params[1] + "\r\n";
        channel->broadcast(modeMsg);
        break ;

    case 't':
        if (active == 1)
            channel->SetModet(true);
        else
            channel->SetModet(false);
        std::string modeMsg = ":" + client->getPrefix() + " MODE " + channelName + " " + message.params[1] + "\r\n";
        channel->broadcast(modeMsg);
        break;

    case 'k': 
        if (key.empty())
        {
            sendToClient(fd, ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
            return;
        }
        if (active == 1)
            channel->SetModek(true, key);
        else
            channel->SetModek(false);
        std::string modeMsg = ":" + client->getPrefix() + " MODE " + channelName + " " + message.params[1] + " " + key + "\r\n";
        channel->broadcast(modeMsg);
        break;
    
    case 'l':
        if (key.empty())
        {
            sendToClient(fd, ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
            return;
        }
        if (active == 1)
            channel->SetModel(true, atoi(key.c_str()));
        else
            channel->SetModel(false);
        std::string modeMsg = ":" + client->getPrefix() + " MODE " + channelName + " " + message.params[1] + " " + key + "\r\n";
        channel->broadcast(modeMsg);
        break;
    
    case 'o': 
        if (key.empty())
        {
            sendToClient(fd, ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
            return;
        }
        Client* target = getClientByNick(key);
        if (!target)
        {
            sendToClient(fd, ERR_NOSUCHNICK(client->getNick(), message.params[2]));
            return;
        }
        int targetfd = target->getFd();
        if (active == 1)
            channel->AddOp(targetfd, target);
        else
            channel->RemoveOp(targetfd);
        std::string modeMsg = ":" + client->getPrefix() + " MODE " + channelName + " " + message.params[1] + " " + target->getNick() + "\r\n";
        channel->broadcast(modeMsg);
        break;
    
    default:
        sendToClient(fd, ERR_UNKNOWNMODE(client->getNick(), std::string(1, modeChar)));
        return;

}

}
