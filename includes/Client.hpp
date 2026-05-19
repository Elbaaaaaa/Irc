/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebella <ebella@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 20:34:00 by ebella            #+#    #+#             */
/*   Updated: 2026/05/19 20:34:00 by ebella           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
    private:

        int         _fd;
        std::string _nick;
        std::string _username;
        std::string _realname;
        std::string _hostname;
        std::string _buffer;
        bool        _authenticated;
        bool        _registered;

    public:

        Client(int fd);
        ~Client();

        int                 getFd() const;
        std::string         getNick() const;
        std::string         getUsername() const;
        std::string         getRealname() const;
        std::string         getHostname() const;
        std::string&        getBuffer();
        bool                isAuthenticated() const;
        bool                isRegistered() const;

        void    setNick(const std::string& nick);
        void    setUsername(const std::string& username);
        void    setRealname(const std::string& realname);
        void    setHostname(const std::string& hostname);
        void    setAuthenticated(bool state);
        void    setRegistered(bool state);

        std::string getPrefix() const;
};

#endif
