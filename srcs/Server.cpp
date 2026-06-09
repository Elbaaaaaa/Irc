/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 21:19:52 by ebella            #+#    #+#             */
/*   Updated: 2026/06/09 11:18:06 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password)
{
    _running = true;
    initSocket();
    initCommands();
}

Server::~Server()
{
}

/* Initialize the socket */
void Server::initSocket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        throw std::runtime_error("socket() failed");
    }
    
    /* Set the server to launch on the same port */
    int opt = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        throw std::runtime_error("setsockopt() failed");
    }

    /* define where the server will listen */
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(_port);
    
    /* bind the socket to the port */
    if (bind(_socket, (struct sockaddr*)&sin, sizeof(sin)) == -1)
    {
        throw std::runtime_error("bind() failed");
    }
    
    /* listen for incoming connections */
    if (listen(_socket, 42) == -1) /* 42 is the maximum number of connections */
    {
        throw std::runtime_error("listen() failed");
    }
    
    /* set the socket to non-blocking */
    if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        throw std::runtime_error("fcntl() failed");
    }
    
    /* add the socket to the pollfd array */
    pollfd server_pfd;
    server_pfd.fd = _socket;
    server_pfd.events = POLLIN;
    server_pfd.revents = 0;

    _fds.push_back(server_pfd);
}

void Server::run()
{
}

void Server::acceptNewClient()
{
}

void Server::removeClient(int fd)
{
}

void Server::writeToClient(int fd)
{
}

Client* Server::getClientByFd(int fd)
{
}

Channel* Server::getChannel(const std::string& name)
{
}

Client* Server::getClientByNick(const std::string& nick)
{
}

void Server::disconnectClient(int fd)
{
}

void Server::createChannel(const std::string& name, Client& op)
{
    Channel* channel = new Channel(name, "");
    channel->AddMember(op.getFd(), &op);
    channel->AddOp(op.getFd(), &op);
    _channels[name] = channel;
    
}

void Server::sendToClient(int fd, const std::string& message)
{

}

void Server::removeClientFromAllChannels(Client& client)
{
    std::map<std::string, Channel*>::iterator it;
    std::vector<std::string> supp;
    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
        Channel* channel = getChannel(it->first);
        if (channel->CheckMember(client.getFd()))
            channel->RemoveMember(client.getFd());
        if (channel->GetMapMember().size() == 0)
            supp.push_back(it->first);
    }
    for (std::vector<std::string>::iterator it2 = supp.begin(); it2 != supp.end(); ++it2)
    {
        removeChannel(*it2);
    }
}

void Server::removeChannel(const std::string& name)
{
}

void Server::handleCommand(int fd, IrcMessage& message)
{
}

void Server::sendToClient(int fd, const std::string& message)
{
    if (fd < 0 || message.empty())
        return;

    std::string full_message = message + "\r\n";

    size_t total_sent = 0;
    while (total_sent < full_message.length())
    {
        ssize_t res = send(fd, full_message.c_str() + total_sent, full_message.length() - total_sent, 0);
        if (res <= 0)
            return;
        total_sent += res;
    }
}


void Server::initCommands()
{

}


// run() — boucle principale du serveur. Elle appelle poll() en boucle, vérifie quels fds sont prêts, et appelle acceptNewClient() ou lit les données des clients existants.

// acceptNewClient() — appelée quand le socket serveur est prêt. Elle fait accept() pour accepter la connexion, crée un nouveau Client, l'ajoute à _clients et _fds.

// removeClient(int fd) — supprime un client de _clients et _fds, ferme son fd avec close().

// writeToClient(int fd) — envoie les données en attente au client. Utile si tu gères un buffer d'envoi.

// getClientByFd(int fd) — cherche dans _clients et retourne le Client* correspondant au fd, ou NULL si pas trouvé.

// getChannel(const std::string& name) — cherche dans _channels et retourne le Channel* correspondant au nom, ou NULL si pas trouvé.

// getClientByNick(const std::string& nick) — parcourt _clients et retourne le Client* dont le nick correspond, ou NULL si pas trouvé.

// disconnectClient(int fd) — déconnecte proprement un client : le retire de tous les channels, puis appelle removeClient.


// sendToClient(int fd, const std::string& message) — envoie une string à un client via send().

// removeClientFromAllChannels(Client& client) — parcourt tous les channels dans _channels et appelle RemoveMember pour ce client sur chacun d'eux. Si un channel devient vide, le supprimer.

// removeChannel(const std::string& name) — supprime le channel de _channels et libère la mémoire avec delete.

// handleCommand(int fd, IrcMessage& message) — cherche la commande dans _commands et l'appelle. Si elle n'existe pas, envoie l'erreur 421 ERR_UNKNOWNCOMMAND.




//initCommands() -- une boucle de ce type, une ligne par commande :
// _commands["NOM"] = &Server::methode;
// Les commandes à enregistrer d'après le plan de travail :

// CAP, PASS, NICK, USER, QUIT
// PING, PRIVMSG, NOTICE
// JOIN, KICK, PART, INVITE, TOPIC, MODE
