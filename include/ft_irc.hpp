#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#include <iostream>
#include <cstring>
#include <csignal>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cmath>
#include <algorithm>
#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"

#define CLIENT(nick, user)							(nick + "!" + user + "@localhost")
#define PRIVMSG(client, target, message)			(":" + client + " PRIVMSG " + target + " :" + message + "\r\n")

// Reply messages
#define RPL_JOIN(client, channel)					(":" + client + " JOIN " + channel + "\r\n")
#define RPL_ENDOFNAMES(client, channel)				(": 366 " + client + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_PART(client, channel)					(":" + client + " PART " + channel + "\r\n")
#define RPL_PARTMESSAGE(client, channel, message)	(":" + client + " PART " + channel + " :" + message +"\r\n")
#define RPL_MODE(client, channel, mode, name)		(":" + client + " MODE " + channel + " " + mode + " " + name + "\r\n")
#define RPL_KICK(client, channel, target, reason)	(":" + client + " KICK " + channel + " " + target + " " + reason + "\r\n")
#define RPL_INVITERCVR(client, invitee, channel)	(":" + client + " INVITE " + invitee + " " + channel + "\r\n")
#define RPL_NICK(oldNick, newNick)					(":" + oldNick + " NICK " + newNick + "\r\n")
#define RPL_TOPIC(client, channel, topic)			(":" + client + " TOPIC " + channel + " :" + topic + "\r\n")
#define RPL_QUIT(client, message)					(":" + client + " QUIT :" + message + "\r\n")
#define RPL_WELCOME(nickname, client)				(": 001 " + nickname + " :Welcome to the IRC world, " + client + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, modes) 	(": 324 " + client + " " + channel + " " + modes + "\r\n")
#define RPL_NOTOPIC(client, channel)				(": 331 " + client + " " + channel + " :No topic is set\r\n")
#define RPL_SEETOPIC(client, channel, topic)		(": 332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_INVITESNDR(client, invitee, channel)	(": 341 " + client + " " + invitee + " " + channel + "\r\n")
#define RPL_NAMEREPLY(nick, channel, nicknames)		(": 353 " + nick + " = " + channel + " :" + nicknames + "\r\n")

// Error messages
#define ERR_TOOMUCHPARAMS(client, cmd)				(": " + client + " " + cmd + " :Too much parameters\r\n")
#define ERR_USERONCHANNEL(nick, channel)			(": 443 " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_NOSUCHNICK(client, nickname) 			(": 401 " + client + " " + nickname + " :No such nick\r\n")
#define ERR_NOSUCHNICKCHAN(server, client, nickname)(":" + server + " 401 " + client + " " + nickname + " :No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(client, channel)			(": 403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel)		(": 404 " + client + " " + channel + " :Cannot send to channel\r\n")
#define ERR_NOTEXTTOSEND(client)					(": 412 " + client + " :No text to send\r\n")

#define ERR_UNKNOWNCOMMAND(client, command)			(": 421 " + client + " " + command + " :Unknown command\r\n")

#define ERR_NONICKNAMEGIVEN(client, nick)			(": 431 " + client + " " + nick + " :No nickname given\r\n")

#define ERR_ERRONEUSNICKNAME(client, nickname)		(": 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nick)				        (": 433 * " + nick + " :Nickname is already in use\r\n")
#define ERR_ERRONEUSUSERNAME(client, username)		(": 432 " + client + " " + username + " :Erroneus username\r\n")
#define ERR_USERNAMEINUSE(client, username)			(": 433 * " + client + " " + username + " :Username is already in use\r\n")
#define ERR_NORECIPIENT(client, command)			(": 411 " + client + " :No recipient given (" + command + ")\r\n")

#define ERR_USERNOTINCHANNEL(client, nick, channel)	(": 441 " + client + " " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(client, channel)			(": 442 " + client + " " + channel + " :You're not on that channel\r\n")
#define ERR_NOTREGISTERED()							(": 451 :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(client, cmd)				(": 461 " + client + " " + cmd + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(client)				(": 462 " + client + " :Unauthorized command (already registered)\r\n")
#define ERR_PASSWDMISMATCH(client)					(": 464 " + client + " :Password incorrect\r\n")
// #define ERR_KEYSET(channel)							(": 467 " + channel + " :Channel key already set\r\n")
#define ERR_CHANNELISFULL(client, channel)			(": 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_UNKNOWNMODE(client, mode)				(": 472 " + client + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_INVITEONLYCHAN(client, channel)			(": 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(client, channel)			(": 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel)		(": 482 " + client + " " + channel + " :You're not channel operator\r\n")

#endif