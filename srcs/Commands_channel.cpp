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
#include "../includes/Channel.hpp"

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
        sendToClient(fd, ":server 461 " + client->getNick() + " JOIN :Not enough parameters\r\n");
        return;
    }

    std::string channelName = message.params[0];
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
    }
    if (!channel->CheckOp(fd))
    {
       sendToClient(fd, ERR_CHANOPRIVSNEEDED(client->getNick(), channelName));
    }
    if (!channel->CheckMember(looserfd))
    {
       sendToClient(fd, ERR_USERNOTINCHANNEL(client->getNick(), channelName));
    }

    std::string kickMsg = ":" + client->getPrefix() + " KICK " + looser->getNick() + " " + channelName + "\r\n";
    channel->broadcast(kickMsg);

}
// KICK c'est expulser un autre client du channel.
// Ce qu'il doit faire :

// Vérifier qu'un channel et un nick cible sont fournis en paramètre (461)
// Vérifier que le channel existe (403)
// Vérifier que le client qui kick est bien membre du channel (442)
// Vérifier que le client qui kick est opérateur (482)
// Vérifier que la cible est bien membre du channel (441)
// Broadcaster le message KICK à tout le channel -----------------------
// Retirer la cible du channel avec RemoveMember
// Si le channel est vide après → le supprimer avec removeChannel

// Le message KICK broadcasted ressemble à :
// :nick!user@host KICK #channel cible :raison
// La raison est optionnelle — c'est message.params[2] si elle existe.







// TOPIC c'est voir ou changer le topic d'un channel.

// Vérifier qu'un channel est fourni (461)
// Vérifier que le channel existe (403)
// Vérifier que le client est membre du channel (442)
// Si pas de deuxième paramètre → juste afficher le topic actuel (332) ou (331) si vide
// Si deuxième paramètre → c'est un changement de topic :

// Vérifier que le mode +t est off OU que le client est opérateur (482)
// Changer le topic avec SetTopic
// Broadcaster le nouveau topic à tout le channel

// Le message broadcasted ressemble à :
// :nick!user@host TOPIC #channel :nouveau topic









// INVITE c'est inviter quelqu'un dans un channel.

// Vérifier qu'un nick et un channel sont fournis (461)
// Vérifier que le channel existe (403)
// Vérifier que le client qui invite est membre du channel (442)
// Vérifier que le client qui invite est opérateur (482)
// Vérifier que la cible existe avec getClientByNick (401)
// Vérifier que la cible n'est pas déjà dans le channel (443)
// Ajouter la cible dans _invited avec AddInvited
// Envoyer le 341 au client qui invite
// Envoyer le message INVITE à la cible

// Le message envoyé à la cible ressemble à :
// :nick!user@host INVITE cible #channel