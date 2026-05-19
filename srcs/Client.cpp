/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebella <ebella@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 20:34:00 by ebella            #+#    #+#             */
/*   Updated: 2026/05/19 20:34:00 by ebella           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(int fd) : _fd(fd), _authenticated(false), _registered(false)
{
}

Client::~Client()
{
}

int Client::getFd() const
{
    return _fd;
}

std::string Client::getNick() const
{
    return _nick;
}

std::string Client::getUsername() const
{
    return _username;
}

std::string Client::getRealname() const
{
    return _realname;
}

std::string Client::getHostname() const
{
    return _hostname;
}

std::string& Client::getBuffer()
{
    return _buffer;
}

bool Client::isAuthenticated() const
{
    return _authenticated;
}

bool Client::isRegistered() const
{
    return _registered;
}

void Client::setNick(const std::string& nick)
{
    _nick = nick;
}

void Client::setUsername(const std::string& username)
{
    _username = username;
}

void Client::setRealname(const std::string& realname)
{
    _realname = realname;
}

void Client::setHostname(const std::string& hostname)
{
    _hostname = hostname;
}

void Client::setAuthenticated(bool state)
{
    _authenticated = state;
}

void Client::setRegistered(bool state)
{
    _registered = state;
}

std::string Client::getPrefix() const
{
    return _nick + "!" + _username + "@" + _hostname;
}
