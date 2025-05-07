#ifndef USER_HPP
# define USER_HPP

#include 	<iostream>
#include 	<map>
#include 	<netinet/in.h>
#include	<sstream>
#include	<fcntl.h>

class Channel;
class Server;

typedef std::map<std::string, Channel *>	ChannelMap;
typedef enum {PASS, NICK, USER, REGISTER} RegistrationState;

class User {
protected:
	std::string	_username;
	std::string _nickname;
	std::string _buffer;
	bool 		_saveBuffer;
	sockaddr_in _addr;
    int         _sockfd;
	RegistrationState _registrationState;

	ChannelMap	_channels;

	User();
public:
	User(int servSockfd);
	virtual ~User();

	void	receiveRequest();
	void	interpretRequest(Server &server);
	void	sendRequest(std::string msg);
	void	sendMsg(std::istringstream &request, std::string &client, Server &server);
	void	quit(std::istringstream &request, std::string &client, Server &server);
	void	joinChannel(std::istringstream &request, std::string &client, Server &server);

	virtual void	leaveChannel(std::istringstream &request, std::string &client, Server &server);

	// Operator
	void	kick(std::istringstream &request, std::string &client, Server &server);
    void    invite(std::istringstream &request, std::string &client, Server &server); 
	// TOPIC
	void	topic(std::istringstream &request, std::string &client, Server &server);
	// MODE
	void	mode(std::istringstream &request, std::string &client, Server &server);
	void	who(std::istringstream &request, std::string &client, Server &server);

	void	checkPass(std::istringstream &request, std::string &client, Server &server);
	void	setUsername(std::istringstream &request, std::string &client, Server &server);
	void	setNickname(std::istringstream &request, std::string &client, Server &server);
	std::string	getUsername() const;
	std::string	getNickname() const;
	sockaddr_in	getAddr() const;
	int			getSockfd() const;
};

#endif