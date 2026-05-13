/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebella <ebella@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 15:26:38 by ebella            #+#    #+#             */
/*   Updated: 2026/05/10 15:26:38 by ebella           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <poll.h>
#include <string>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

#include "Client.hpp"
#include "Channel.hpp"
#include "IrcReply.hpp"
#include "IrcMessage.hpp"



class Server
{
    private:
    
        int _port;
        int _socket;

        bool _running;

        std::string _password;
        
        std::vector<pollfd> _fds;
        
        std::map<int, Client*> _clients;
        std::map<std::string, Channel*> _channels;
        std::map<std::string, void (Server::*)(int, IrcMessage&)> _commands;

        void initSocket();
        void acceptNewClient();
        void removeClient(int fd);
        void writeToClient(int fd);
        void handleCommand(int fd, IrcMessage& message);

    public:

        Server(int port, std::string password);
        ~Server();

        void run();
        
        Client* getClientByFd(int fd);
        Channel* getChannel(const std::string& name);
        Client* getClientByNick(const std::string& nick);

        void disconnectClient(int fd);
        void createChannel(const std::string& name, Client& op);
        void sendToClient(int fd, const std::string& message);
        void removeClientFromAllChannels(Client& client);
        void removeChannel(const std::string& name);
};

#endif