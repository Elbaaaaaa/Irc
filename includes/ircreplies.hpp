/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircreplies.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntamacha <ntamacha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 17:26:05 by ntamacha          #+#    #+#             */
/*   Updated: 2026/06/06 17:26:05 by ntamacha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ressources : https://www.rfc-editor.org/info/rfc1459/#section-6

#ifndef IRC_REPLY_HPP
# define IRC_REPLY_HPP
 
# include <string>
 
# define SERVER_NAME "ircserv"
 
// 401 - Nick introuvable
# define ERR_NOSUCHNICK(nick, target) \
    (":") + SERVER_NAME + " 401 " + (nick) + " " + (target) + " :No such nick/channel\r\n"
 
// 403 - Channel invalide
# define ERR_NOSUCHCHANNEL(nick, channel) \
    (":") + SERVER_NAME + " 403 " + (nick) + " " + (channel) + " :No such channel\r\n"
 
// 404 - Impossible d'envoyer au channel
# define ERR_CANNOTSENDTOCHAN(nick, channel) \
    (":") + SERVER_NAME + " 404 " + (nick) + " " + (channel) + " :Cannot send to channel\r\n"
 
// 421 - Commande inconnue
# define ERR_UNKNOWNCOMMAND(nick, command) \
    (":") + SERVER_NAME + " 421 " + (nick) + " " + (command) + " :Unknown command\r\n"
 
// 431 - Pas de nickname fourni
# define ERR_NONICKNAMEGIVEN(nick) \
    (":") + SERVER_NAME + " 431 " + (nick) + " :No nickname given\r\n"
 
// 432 - Nickname invalide
# define ERR_ERRONEUSNICKNAME(nick, target) \
    (":") + SERVER_NAME + " 432 " + (nick) + " " + (target) + " :Erroneus nickname\r\n"
 
// 433 - Nickname déjà utilisé
# define ERR_NICKNAMEINUSE(nick, target) \
    (":") + SERVER_NAME + " 433 " + (nick) + " " + (target) + " :Nickname is already in use\r\n"
 
// 441 - L'utilisateur n'est pas dans le channel
# define ERR_USERNOTINCHANNEL(nick, target, channel) \
    (":") + SERVER_NAME + " 441 " + (nick) + " " + (target) + " " + (channel) + " :They aren't on that channel\r\n"
 
// 442 - Le client n'est pas dans le channel
# define ERR_NOTONCHANNEL(nick, channel) \
    (":") + SERVER_NAME + " 442 " + (nick) + " " + (channel) + " :You're not on that channel\r\n"
 
// 443 - L'utilisateur est déjà dans le channel
# define ERR_USERONCHANNEL(nick, target, channel) \
    (":") + SERVER_NAME + " 443 " + (nick) + " " + (target) + " " + (channel) + " :is already on channel\r\n"
 
// 451 - Client pas encore enregistré
# define ERR_NOTREGISTERED(nick) \
    (":") + SERVER_NAME + " 451 " + (nick) + " :You have not registered\r\n"
 
// 461 - Pas assez de paramètres
# define ERR_NEEDMOREPARAMS(nick, command) \
    (":") + SERVER_NAME + " 461 " + (nick) + " " + (command) + " :Not enough parameters\r\n"
 
// 462 - Déjà enregistré
# define ERR_ALREADYREGISTRED(nick) \
    (":") + SERVER_NAME + " 462 " + (nick) + " :You may not reregister\r\n"
 
// 464 - Mauvais mot de passe serveur
# define ERR_PASSWDMISMATCH(nick) \
    (":") + SERVER_NAME + " 464 " + (nick) + " :Password incorrect\r\n"
 
// 467 - Clé du channel déjà définie
# define ERR_KEYSET(nick, channel) \
    (":") + SERVER_NAME + " 467 " + (nick) + " " + (channel) + " :Channel key already set\r\n"
 
// 471 - Channel plein (+l)
# define ERR_CHANNELISFULL(nick, channel) \
    (":") + SERVER_NAME + " 471 " + (nick) + " " + (channel) + " :Cannot join channel (+l)\r\n"
 
// 472 - Mode inconnu
# define ERR_UNKNOWNMODE \
    (":") + SERVER_NAME + " 472 " + (nick) + " " + (c) + " :is unknown mode char to me\r\n"
 
// 473 - Channel invite only (+i)
# define ERR_INVITEONLYCHAN(nick, channel) \
    (":") + SERVER_NAME + " 473 " + (nick) + " " + (channel) + " :Cannot join channel (+i)\r\n"
 
// 475 - Mauvais mot de passe channel (+k)
# define ERR_BADCHANNELKEY(nick, channel) \
    (":") + SERVER_NAME + " 475 " + (nick) + " " + (channel) + " :Cannot join channel (+k)\r\n"
 
// 482 - Pas opérateur du channel
# define ERR_CHANOPRIVSNEEDED(nick, channel) \
    (":") + SERVER_NAME + " 482 " + (nick) + " " + (channel) + " :You're not channel operator\r\n"


// 331 - Pas de topic
# define RPL_NOTOPIC(nick, channel) \
    (":") + SERVER_NAME + " 331 " + (nick) + " " + (channel) + "  channel->GetTopic():No topic is set\r\n"
 
// 332 - Topic du channel
# define RPL_TOPIC(nick, channel, topic) \
    (":") + SERVER_NAME + " 332 " + (nick) + " " + (channel) + " :" + (topic) + "\r\n"
 
// 341 - Invitation réussie
# define RPL_INVITING(nick, target, channel) \
    (":") + SERVER_NAME + " 341 " + (nick) + " " + (target) + " " + (channel) + "\r\n"
 
// 353 - Liste des membres du channel
# define RPL_NAMREPLY(nick, channel, names) \
    (":") + SERVER_NAME + " 353 " + (nick) + " = " + (channel) + " :" + (names) + "\r\n"
 
// 366 - Fin de la liste des membres
# define RPL_ENDOFNAMES(nick, channel) \
    (":") + SERVER_NAME + " 366 " + (nick) + " " + (channel) + " :End of /NAMES list\r\n"
 
// 324 - Modes du channel
# define RPL_CHANNELMODEIS(nick, channel, mode) \
    (":") + SERVER_NAME + " 324 " + (nick) + " " + (channel) + " " + (mode) + "\r\n"
 
#endif // IRC_REPLY_HPP