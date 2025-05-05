#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <vector>

class User;
class Channel;

typedef std::map<std::string, Channel *>	ChannelMap;
typedef std::map<std::string, User *>		UserMap;
typedef std::map<int, User *>				UserFdMap;

class Server {
private:
	int	_sockfd;
	sockaddr_in	_addr;
	std::string	_password;

	ChannelMap	_channels;
	UserMap		_users;
	UserFdMap	_usersfd;
	
	bool	_toLeave;

	Server();
	
	void	init(std::string port);
public:
	typedef void (User::*CommandHandler)(std::istringstream &, std::string &, Server &);
	typedef std::map<std::string, CommandHandler> CommandMap;
	
	Server(std::string port, std::string password);
	Server(const Server &src);
	Server	&operator=(const Server &src);
	~Server();
	
	void	run();
	CommandMap	init_commands();
	std::string	getPassword() const;
	Channel	*getChannel(std::string &name);
	User	*getUser(std::string &name);
	ChannelMap	&getChannels();
	UserMap	&getUsers();
	void	toLeave();
};


#endif