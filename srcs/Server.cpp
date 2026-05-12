/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebella <ebella@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 21:19:52 by ebella            #+#    #+#             */
/*   Updated: 2026/05/11 21:19:52 by ebella           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password)
{
    _running = true;
    initSocket();
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
}

void Server::sendToClient(int fd, const std::string& message)
{
}

void Server::removeClientFromAllChannels(Client& client)
{
}

void Server::removeChannel(const std::string& name)
{
}

void Server::handleCommand(int fd, IrcMessage& message)
{
}

void Server::sendToClient(int fd, const std::string& message)
{
}