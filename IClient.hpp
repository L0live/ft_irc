#ifndef ICLIENT_HPP
# define ICLIENT_HPP

#include "Channel.hpp"
#include <iostream>
#include <map>

class IClient {
private:
	std::string	_username;
	std::string _nickname;
	std::string _ip_adress;
	std::map<std::string, Channel *>	_channels;

	IClient();
public:
	IClient(std::string &ip_adress);
	IClient(std::string &ip_adress, std::string &username);
	// IClient(std::string &ip_adress, std::string &username, std::string &nickname);
	virtual ~IClient();

	void	sendChannelMsg(std::string &msg);
	void	sendPrivateMsg(std::string &msg, std::string &target);
	void	joinChannel(Channel *channel);
	virtual void	leaveChannel(std::string &channel) = 0;

	void	setUsername(std::string &username);
	void	setNickname(std::string &nickname);
	std::string	getUsername() const;
	std::string	getNickname() const;
	std::string	getIpAdress() const;
};


#endif