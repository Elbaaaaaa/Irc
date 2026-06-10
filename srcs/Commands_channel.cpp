/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands_channel.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 20:07:34 by ntamacha          #+#    #+#             */
/*   Updated: 2026/05/19 20:07:34 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/Server.hpp"

void Server::JOIN(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;

    if (message.params.empty())
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " JOIN :Not enough parameters\r\n");
        return;
    }

    std::string channelName = message.params[0];
    std::string key = (message.params.size() >= 2) ? message.params[1] : "";

    if (channelName[0] != '#')
    {
        sendToClient(fd, ":server 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    Channel* channel = getChannel(channelName);
    if (!channel)
    {
        createChannel(channelName, *client);
        channel = getChannel(channelName);
    }
    else
    {
        if (channel->CheckMember(fd))
            return;

        if (channel->GetModei() && !channel->CheckInvited(fd))
        {
            sendToClient(fd, ":server 473 " + client->getNick() + " " + channelName + " :Cannot join channel (+i)\r\n");
            return;
        }

        if (channel->GetModek() && channel->Getkey() != key)
        {
            sendToClient(fd, ":server 475 " + client->getNick() + " " + channelName + " :Cannot join channel (+k)\r\n");
            return;
        }

        if (channel->GetModel() && (int)channel->GetMapMember().size() >= channel->GetLimitValue())
        {
            sendToClient(fd, ":server 471 " + client->getNick() + " " + channelName + " :Cannot join channel (+l)\r\n");
            return;
        }

        channel->AddMember(fd, client);
    }

    std::string joinMsg = ":" + client->getPrefix() + " JOIN " + channelName + "\r\n";
    channel->broadcast(joinMsg);
    if (!channel->GetTopic().empty())
    {
        sendToClient(fd, ":server 332 " + client->getNick() + " " + channelName + " :" + channel->GetTopic() + "\r\n");
    }


    std::string namesList = "";
    const std::map<int, Client*>& members = channel->GetMapMember();
    for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); it++)
    {
        if (channel->CheckOp(it->first))
            namesList += "@";
        namesList += it->second->getNick() + " ";
    }
    sendToClient(fd, ":server 353 " + client->getNick() + " = " + channelName + " :" + namesList + "\r\n");
    sendToClient(fd, ":server 366 " + client->getNick() + " " + channelName + " :End of /NAMES list\r\n");
}


void Server::PART(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;

    if (message.params.empty())
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " PART :Not enough parameters\r\n");
        return;
    }

    std::string channelName = message.params[0];

    if (channelName[0] != '#')
    {
        sendToClient(fd, ":server 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
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
    std::string partMsg = ":" + client->getPrefix() + " PART " + channelName + "\r\n";
    channel->broadcast(partMsg);
    channel->RemoveMember(fd);
    if (channel->GetMapMember().size() == 0)
    {
        removeChannel(channelName);
        return ;
    }

}

void Server::KICK(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;

    if (message.params.empty())
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " KICK :Not enough parameters\r\n");
        return;
    }

    std::string channelName = message.params[0];
    if (message.params.size() < 2)
    {
        sendToClient(fd, ERR_NEEDMOREPARAMS(client->getNick(), channelName));
        return;
    }
    std::string raison = (message.params.size() >= 3) ? message.params[2] : "No reason";
    Client* looser = getClientByNick(message.params[1]);
    if (!looser)
    {
        sendToClient(fd, ERR_NOSUCHNICK(client->getNick(), message.params[1]));
        return;
    }
    int looserfd = looser->getFd();

    if (channelName[0] != '#')
    {
        sendToClient(fd, ":server 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
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
    if (!channel->CheckMember(looserfd))
    {
       sendToClient(fd, ERR_USERNOTINCHANNEL(client->getNick(), looser->getNick(),channelName));
       return;
    }

    std::string kickMsg = ":" + client->getPrefix() + " KICK " + channelName + " " + looser->getNick() + " :" + raison + "\r\n";
    channel->broadcast(kickMsg);

    channel->RemoveMember(looserfd);

    if (channel->GetMapMember().size() == 0)
    {
        removeChannel(channelName);
        return ;
    }

}
 

void Server::TOPIC(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;
    if (message.params.empty())
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " TOPIC :Not enough parameters\r\n");
        return;
    }

    std::string channelName = message.params[0];
    if (channelName[0] != '#')
    {
        sendToClient(fd, ":server 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }

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

    std::string newtopic = (message.params.size() >= 2) ? message.params[1] : "";
    if (newtopic.empty())
    {
        if (channel->GetTopic().empty())
        {
            sendToClient(fd, RPL_NOTOPIC(client->getNick(), channelName));
            return;
        }
        sendToClient(fd, RPL_TOPIC(client->getNick(), channelName, channel->GetTopic()));
        return ;
    }
    if (channel->GetModet() && !channel->CheckOp(fd))
    {
        sendToClient(fd, ERR_CHANOPRIVSNEEDED(client->getNick(), channelName));
        return ;
    }
    channel->SetTopic(newtopic);

    std::string topicMsg = ":" + client->getPrefix() + " TOPIC " + channelName +  " :" + newtopic + "\r\n";
    channel->broadcast(topicMsg);

}


void Server::INVITE(int fd, IrcMessage& message)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;

    if (message.params.empty())
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " INVITE :Not enough parameters\r\n");
        return;
    }

    std::string channelName = message.params[0];
    if (message.params.size() < 2)
    {
        sendToClient(fd, ERR_NEEDMOREPARAMS(client->getNick(), channelName));
        return;
    }
    Client* target = getClientByNick(message.params[1]);
    if (!target)
    {
        sendToClient(fd, ERR_NOSUCHNICK(client->getNick(), message.params[1]));
        return;
    }
    int targetfd = target->getFd();

    if (channelName[0] != '#')
    {
        sendToClient(fd, ":server 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
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

    if (channel->CheckMember(targetfd))
    {
       sendToClient(fd, ERR_USERONCHANNEL(client->getNick(), target->getNick(), channelName));
       return;
    }

    channel->AddInvited(targetfd, target);
    sendToClient(fd, RPL_INVITING(client->getNick(), target->getNick(), channelName));

    std::string inviteMsg = ":" + client->getPrefix() + " INVITE " + target->getNick() + " " + channelName + "\r\n";
    sendToClient(targetfd, inviteMsg);
}
