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

        if (channel->GetModel() &&
            (int)channel->GetMapMember().size() >= channel->GetLimitValue())
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