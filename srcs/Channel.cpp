/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 14:08:09 by ntamacha          #+#    #+#             */
/*   Updated: 2026/05/13 14:08:09 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string const& name, std::string const& topic) : _name(name), _topic(topic), _i(false), _t(false), _l(false), _limitValue(0), _k(false), _key("")
{
}
Channel::~Channel()
{}
void Channel::AddMember(int fd, Client* client)
{
    _members[fd] = client;
}
void Channel::RemoveMember(int fd)
{
    if (CheckMember(fd))
        _members.erase(fd);
    if (CheckOp(fd))
        _operators.erase(fd);
    if (CheckInvited(fd))
        _invited.erase(fd);
}
bool Channel::CheckMember(int fd)
{
    if (_members.count(fd))
        return(true);
    else
        return(false);
}
void Channel::AddOp(int fd, Client* client)
{
    _operators[fd] = client;
}
void Channel::RemoveOp(int fd)
{
    if (CheckOp(fd))
        _operators.erase(fd);
}
bool Channel::CheckOp(int fd)
{
    if (_operators.count(fd))
        return(true);
    else
        return(false);
}


void Channel::AddInvited(int fd, Client* client)
{
    _invited[fd] = client;
}
void Channel::RemoveInvited(int fd)
{
    if (CheckInvited(fd))
        _invited.erase(fd);
}
bool Channel::CheckInvited(int fd)
{
    if (_invited.count(fd))
        return(true);
    else
        return(false);
}


const std::map<int, Client*> &Channel::GetMapMember() const
{
    return (this->_members);
}
const std::map<int, Client*> &Channel::GetMapOp() const
{
    return(this->_operators);
}

const std::map<int, Client*> &Channel::GetMapInvited() const
{
    return(this->_invited);
}
std::string Channel::GetName() const
{
    return (this->_name);
}
std::string Channel::GetTopic() const
{
    return(this->_topic);
}
void Channel::SetTopic(std::string const& topic)
{
    this->_topic = topic;
}

void Channel::SetModei(bool state)
{
    this->_i = state;
}
void Channel::SetModet(bool state)
{
    this->_t = state;
}
void Channel::SetModel(bool state, int limitValue)
{
    this->_l = state;
    if (this->_l)
        this->_limitValue = limitValue;
    else
        this->_limitValue = 0;
}
void Channel::SetModek(bool state, std::string const& key)
{
    this->_k = state;
    if (this->_k)
    {
        this->_key = key;
    }
    else
        this->_key = "";
}
bool Channel::GetModei() const
{
    return(this->_i);
}
bool Channel::GetModet() const
{
    return(this->_t);
}
bool Channel::GetModel() const
{
    return(this->_l);
}
bool Channel::GetModek() const
{
    return(this->_k);
}
int Channel::GetLimitValue() const
{
    return(this->_limitValue);
}
std::string const& Channel::Getkey() const
{
    return(this->_key);
}

void broadcast(const std::string& msg, int except_fd)
{
    std::map<int, Client*>::iterator it;
    for (it = _members.begin(); it != _members.end(); it++)
    {
        if (it->first == except_fd)
            continue ; 
        
        sendToClient(it->first, msg);
    }
}