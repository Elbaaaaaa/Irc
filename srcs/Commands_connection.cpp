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

/*PASS, NICK, USER, QUIT, CAP

Commands_connection.cpp

CAP, PASS, NICK, USER, QUIT
Attention : Client.hpp a _authenticated + _registered — il faudra les deux pour le welcome 001
Pas de _password dans Client → tu devras le stocker temporairement, soit en ajoutant un champ dans Client, soit via une std::map<int, std::string> dans Server

CAP — c'est juste vérifier le sous-commande (LS, REQ, END) et répondre en conséquence.
CAP LS → répondre CAP * LS : (liste vide)
CAP REQ → répondre CAP * NAK : (refus)
CAP END → ne rien faire
*/


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
    int i = 0;
    if (nick[i] != '_' && !isalpha(nick))
        return (0);
    for (i = 1; nick.size() > i; i++)
    {}

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


    std::map<std::string, Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* c = getClientByFd(it->first);
        if (c->getNick() == nick)
        {
            sendToClient(fd, ERR_NICKNAMEINUSE(client->getNick(), fd));
            return ;
        }
    }
    client->setNick(nick);
}

