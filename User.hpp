#ifndef USER_HPP
# define USER_HPP

#include 	<iostream>
#include 	<map>
#include 	<netinet/in.h>
#include	<sstream>

class Channel;
class Server;

typedef std::map<std::string, Channel *>	ChannelMap;

class User {
protected:
	std::string	_username;
	std::string _nickname;
    sockaddr_in _addr;
    int         _sockfd;

	ChannelMap	_channels;

	User();
public:
	User(int servSockfd);
	virtual ~User();

	std::string	receiveRequest();
	void	interpretRequest(std::istringstream &request, Server &server);
	void	sendChannelMsg(std::istringstream &request);
	void	sendPrivateMsg(std::istringstream &request);
	void	joinChannel(std::istringstream &request);

	virtual void	leaveChannel(std::istringstream &request);

	void	checkPass(std::istringstream &request);
	void	setUsername(std::istringstream &request);
	void	setNickname(std::istringstream &request);
	std::string	getUsername() const;
	std::string	getNickname() const;
	sockaddr_in	getAddr() const;
	int	        getSockfd() const;
};

#endif