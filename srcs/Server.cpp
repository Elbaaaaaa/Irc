/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebella <ebella@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 21:19:52 by ebella            #+#    #+#             */
/*   Updated: 2026/06/09 11:31:13 by ebella           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <iterator>
#include <map>
#include <netinet/in.h>
#include <new>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

Server::Server(int port, std::string password) : _port(port), _password(password)
{
    _running = true;
    initSocket();
    //initCommands();
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
	while (_running)
	{
		int ret = poll(&_fds[0], _fds.size(), -1);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll() failed");
		}

		for(size_t i = _fds.size(); i-- > 0;)
		{
			if (!(_fds[i].revents & POLLIN))
				continue;

			int fd = _fds[i].fd;
			if (fd == _socket)
				acceptNewClient();
			else
				readFromClient(fd);
		}
	}
}

void Server::readFromClient(int fd)
{
	char buf[512];
	
	ssize_t bytes = recv(fd, buf, sizeof(buf) - 1, 0);
	if (bytes <= 0)
	{
		disconnectClient(fd);
		return;
	}

	buf[bytes] = '\0';

	Client* client = getClientByFd(fd);
	if (!client)
		return;

	client->getBuffer() += buf;

	std::string& buffer = client->getBuffer();
	size_t pos;
	while((pos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string line = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);
		if (!line.empty())
		{
			IrcMessage msg = Parser::parseMessage(line);
			if (!msg.command.empty())
				handleCommand(fd, msg);
		}
	}
}

void Server::acceptNewClient()
{
	struct sockaddr_in sin = {0};
	socklen_t len = sizeof(sin);

	int client_fd = accept(_socket, (struct sockaddr *)&sin, &len);
	if (client_fd < 0)
	{
		perror("accept() failed");
		return;
	}
	if (fcntl( client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        close(client_fd);
		return;
    }
	Client* client = new Client(client_fd);

	_clients[client_fd] = client;

	pollfd client_pfd;
    client_pfd.fd = client_fd;
    client_pfd.events = POLLIN;
    client_pfd.revents = 0;

	_fds.push_back(client_pfd);
}

void Server::removeClient(int fd)
{
	Client* client = getClientByFd(fd);
	if (!client)
		return;

	removeClientFromAllChannels(*client);
	close(fd);
	delete client;
	_clients.erase(fd);
	
	for(std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_fds.erase(it);
			return;
		}
	}
}

Client* Server::getClientByFd(int fd)
{
	std::map<int,Client*>::iterator it = _clients.find(fd);
	if ( it == _clients.end())
		return NULL;

	return it->second;
}

Channel* Server::getChannel(const std::string& name)
{
	std::map<std::string,Channel*>::iterator it = _channels.find(name);
	if ( it == _channels.end())
		return NULL;

	return it->second;
}

Client* Server::getClientByNick(const std::string& nick)
{
	std::map<int,Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNick() == nick)
			return it->second;
	}
	return NULL;
}

void Server::disconnectClient(int fd)
{
	sendToClient(fd, "ERROR :Closing connection");
	removeClient(fd);
}

void Server::createChannel(const std::string& name, Client& op)
{
    Channel* channel = new Channel(name, "");
    channel->AddMember(op.getFd(), &op);
    channel->AddOp(op.getFd(), &op);
    _channels[name] = channel;
    
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
	std::map<std::string,Channel*>::iterator it = _channels.find(name);
	if ( it == _channels.end())
		return;

	delete it->second;
	_channels.erase(name);
}

void Server::handleCommand(int fd, IrcMessage& message)
{
	std::map<std::string, void(Server::*)(int, IrcMessage&)>::iterator it = _commands.find(message.command);
	if (it == _commands.end())
	{
		Client *client = getClientByFd(fd);
		if (!client)
			return;
		std::string nick = client->getNick();
		if (nick.empty())
			return;
		sendToClient(fd, std::string(ERR_UNKNOWNCOMMAND(nick, message.command)));
		return;
	}
	(this->*(it->second))(fd, message);
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
	_commands["JOIN"] = &Server::JOIN;
	_commands["KICK"] = &Server::KICK;
	_commands["PART"] = &Server::PART;
	_commands["INVITE"] = &Server::INVITE;
	_commands["TOPIC"] = &Server::TOPIC;
	_commands["MODE"] = &Server::MODE;

	void JOIN(int fd, IrcMessage& message);
	void KICK(int fd, IrcMessage& message);
	void PART(int fd, IrcMessage& message);
	void INVITE(int fd, IrcMessage& message);
	void TOPIC(int fd, IrcMessage& message);
	void MODE(int fd, IrcMessage& message);
}


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
