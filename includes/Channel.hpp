/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 20:04:27 by ntamacha          #+#    #+#             */
/*   Updated: 2026/05/11 20:04:27 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include "Client.hpp"
#include <string>
#include <map>

class Channel
{

private :

    std::string const _name;
    std::string _topic;
    std::map<int, Client*> _members;
    std::map<int, Client*> _operators;
    std::map<int, Client*> _invited;
    bool _i;
    bool _t;
    bool _k;
    std::string _key;
    bool _l;
    int _limitValue;


public :

    Channel(std::string const& name, std::string const& topic);
    ~Channel();


    void AddMember(int fd, Client* client);
    void RemoveMember(int fd);
    bool CheckMember(int fd);


    void AddOp(int fd, Client* client);
    void RemoveOp(int fd);
    bool CheckOp(int fd);

    void AddInvited(int fd, Client* client);
    void RemoveInvited(int fd);
    bool CheckInvited(int fd);

    const std::map<int, Client*> &GetMapMember() const;
    const std::map<int, Client*> &GetMapOp() const;
    const std::map<int, Client*> &GetMapInvited() const;
    std::string GetName() const;
    std::string GetTopic() const;
    void SetTopic(std::string const& topic);
    
    void SetModei(bool state);
    void SetModet(bool state);
    void SetModel(bool state, int limitValue);
    void SetModek(bool state, std::string const& key = "");

    bool GetModei() const;
    bool GetModet() const;
    bool GetModel() const;
    bool GetModek() const;
    int GetLimitValue() const;
    std::string const& Getkey() const;
    
    void broadcast(const std::string& msg, int except_fd = -1);


};

#endif