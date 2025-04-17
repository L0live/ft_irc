#ifndef USER_HPP
# define USER_HPP

#include <iostream>
#include <string>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>

class Channel;

class User {
private:
	std::string	_username;
	std::string _nickname;
    sockaddr_in _addr;
    int         _sockfd;

	std::map<std::string, Channel *>	_channels;

	Channel	*_activeChannel;

	User();
public:
	User(int sockfd, sockaddr &_addr);
	virtual ~User();

	void	sendChannelMsg(std::string &msg);
	// void	sendChannelMsg(std::string &msg, std::string &channel);
	// void	sendChannelMsg(std::string &msg, Channel *channel);
	void	sendPrivateMsg(std::string &msg, std::string &target);
	void	joinChannel(std::string &channel);
	virtual void	leaveChannel();

	void	setUsername(std::string &username);
	void	setNickname(std::string &nickname);
	std::string	getUsername() const;
	std::string	getNickname() const;
	sockaddr    getAddr() const;
	int	        getSockfd() const;
};

#endif