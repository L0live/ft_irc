#ifndef SERVER_HPP
# define SERVER_HPP

#include "Channel.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <cmath>

class Server {
private:
	int	_sockfd;
	sockaddr_in	_serv_addr;
	std::string	_password;

	std::map<std::string, Channel *>	_channels;
	std::map<std::string, User *>	_users;
	
	Server();

	void	init(std::string port);
public:
	Server(std::string port, std::string password);
	Server(const Server &src);
	Server	&operator=(const Server &src);
	~Server();
	
	void	run();
	void	sendPrivateMsg(User *user, const std::string &msg, const std::string &target);
	void	joinChannel(User *user, const std::string &channel);
	void	addChannel(User *user, Channel *channel);
	void	popChannel(const std::string &channel);
};


#endif