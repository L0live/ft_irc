#ifndef USER_HPP
# define USER_HPP

#include "ft_irc.hpp"

class User {
private:
	std::string	_username;
	std::string _nickname;
    sockaddr_in _addr;
    int         _sockfd;

	ChannelMap	_channels;

	User();
public:
	User(int servSockfd);
	virtual ~User();

	void	sendChannelMsg(std::string &channel, std::string &msg);
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