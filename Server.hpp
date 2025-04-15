#ifndef SERVER_HPP
# define SERVER_HPP

#include "IClient.hpp"
#include "Channel.hpp"
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cmath>

class Server {
private:
	std::string _port;
	std::string	_password;

	std::map<std::string, Channel *>	_channels;
	std::map<std::string, IClient *>	_users;
	
	Server();
public:
	Server(std::string port, std::string password);
	Server(const Server &src);
	Server	&operator=(const Server &src);
	~Server();
	
	void	run();
	void	sendPrivateMsg(IClient *user, const std::string &msg, const std::string &target);
	void	joinChannel(IClient *user, const std::string &channel);
	void	addChannel(IClient *user, Channel *channel);
	void	popChannel(const std::string &channel);
};


#endif