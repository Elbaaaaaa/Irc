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
    if (message.params.size() < 2)
    {
        sendToClient(fd, ":server 461 " + client->getNick() + " MODE :Not enough parameters\r\n");
        return;
    }
    std::string channelName = message.params[0];
    char modeChar = message.params[1][1];
    char 
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
    if(mode.size() != 2)
    {
        sendToClient(fd, ERR_UNKNOWNMODE(client->getNick(), channelName));
        return;
    }
    
}


// MODE c'est changer les paramètres d'un channel.
// Ce qu'il doit faire :

// Parser le signe + ou - pour savoir si on active ou désactive le mode
// Parser la lettre du mode i, t, k, l, o et appliquer le bon changement :

// +i / -i → SetModei
// +t / -t → SetModet
// +k / -k → SetModek avec le mot de passe en paramètre
// +l / -l → SetModel avec la limite en paramètre
// +o / -o → AddOp ou RemoveOp avec le nick en paramètre


// Broadcaster le changement de mode à tout le channel
// Si mode inconnu → erreur 472