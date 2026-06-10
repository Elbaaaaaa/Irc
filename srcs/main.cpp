/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 12:21:49 by ntamacha          #+#    #+#             */
/*   Updated: 2026/06/10 12:21:49 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include "../includes/Server.hpp"
#include <csignal>

bool g_running = true;

void signalHandler(int sig)
{
    (void)sig;
    g_running = false;
}

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "./ircserv <port> <password>" << std::endl;
        return (1);
    }
    int port = atoi(av[1]);
    if (port == 0 || port < 1024 || port > 65535)
    {
        std::cerr << "invalid port" << std::endl;
        return 1;
    }

    std::string pwd = av[2];
    if (pwd.empty())
    {
        std::cerr << "./ircserv <port> <password>" << std::endl;
        return (1);
    }


    try { 
        Server server(port, pwd);
        signal(SIGINT, signalHandler);
        server.run();
    }
    catch(std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;

}