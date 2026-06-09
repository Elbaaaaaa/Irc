/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebella <ebella@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 15:26:38 by ebella            #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2026/06/09 11:33:43 by ebella           ###   ########.fr       */
=======
/*   Updated: 2026/06/09 12:00:33 by ntamacha         ###   ########.fr       */
>>>>>>> 1f3a94b (commit)
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
#include "ircreplies.hpp"
#include "IrcMessage.hpp"
#include "Parser.hpp"




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
        std::map<int, std::string> _clientPassword;
        std::map<std::string, void (Server::*)(int, IrcMessage&)> _commands;

        void initSocket();
        void acceptNewClient();
        void initCommands();
        void removeClient(int fd);
        void writeToClient(int fd);
        void handleCommand(int fd, IrcMessage& message);
        void readFromClient(int fd);

        void JOIN(int fd, IrcMessage& message);
        void KICK(int fd, IrcMessage& message);
        void PART(int fd, IrcMessage& message);
        void INVITE(int fd, IrcMessage& message);
        void TOPIC(int fd, IrcMessage& message);
        void MODE(int fd, IrcMessage& message);

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