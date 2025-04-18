#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"

class Server {
private:
	int	_sockfd;
	sockaddr_in	_addr;
	std::string	_password;

	ChannelMap	_channels;
	UserMap	_users;
	
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