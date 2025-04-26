#include "User.hpp"
#include "ft_irc.hpp"

User::User(int servSockfd) {
	std::cout << "Waiting for connections..." << std::endl;
	socklen_t addr_len = sizeof(_addr);
	_sockfd = accept(servSockfd, (sockaddr *)&_addr, &addr_len); // on attend jusqu'a ce que le client se connecte
	if (_sockfd < 0)
		return ;

	std::cout << "Client connected: " << inet_ntoa(_addr.sin_addr) << ":" << ntohs(_addr.sin_port) << std::endl;


}

User::~User() {
	if (_sockfd != -1) {
		close(_sockfd);
	}
}

std::string User::receiveRequest() {
	
	char	buffer[1024];
	int		tmp;

	memset(buffer, '\0', sizeof(buffer));
	tmp = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
	if (tmp <= 0) // vrmt necessaire ?
		return ("");	
	std::cout << "Received: " << buffer << std::endl;
	return std::string(buffer);
}

void User::interpretRequest(std::istringstream &request, Server &server) {
	static Server::CommandMap commands = server.init_commands();
	std::string client = CLIENT(_nickname, _username);

	std::string	tokenLine;
	std::getline(request, tokenLine);
	while (!tokenLine.empty()) {
		std::istringstream	requestLine(tokenLine);
		std::string	token;
		if (requestLine >> token) {
			Server::CommandMap::iterator	it = commands.find(token);
			if (it != commands.end())
				(this->*(it->second))(requestLine, client, server);
		}
		std::getline(request, tokenLine);
	}
}

void User::sendMsg(std::istringstream &request, std::string &client, Server &server) {
	std::string target;
	if (!(request >> target)) // Error: no target
		return ;

	std::string msg = PRIVMSG(client, target, request.str());
	if (target[0] == '#') {
		ChannelMap::iterator	it = _channels.find(target);
		if (it == _channels.end()) // Error: not in channel
			return ;
		it->second->sendAllUser(msg);
		std::cout << "Sended: " << msg << std::endl;
	} else {
		UserMap &users = server.getUsers();
		UserMap::iterator	it = users.find(target);
		if (it == users.end()) // Error: nick not found
			return ;
		send(it->second->getSockfd(), msg.c_str(), msg.size(), 0);
		std::cout << "Sended: " << msg << std::endl;
	}
	
}


void User::joinChannel(std::istringstream &request, std::string &client, Server &server) {
	std::string	target;
	
	if (!(request >> target)) // Error: no target
		return ;
	ChannelMap &allChannels = server.getChannels();
	ChannelMap::iterator	it = allChannels.find(target);
	std::string	msg = RPL_JOIN(client, target);
	if (it == allChannels.end()) {
		Channel *newChannel = new Channel(target, this);
		_channels.insert(std::make_pair(target, newChannel));
		allChannels.insert(std::make_pair(target, newChannel));
	} else {
		if (it->second->getByInvitation()) // Error: channel is invite only
			return ;
		it->second->addUser(this);
		_channels.insert(std::make_pair(target, it->second));
	}
	send(_sockfd, msg.c_str(), msg.size(), 0);
	std::cout << "Sended: " << msg << std::endl;
	// #define ERR_CHANNELISFULL(client, channel)			(": 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
	// #define ERR_INVITEONLYCHAN(client, channel)			(": 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
	// #define ERR_BADCHANNELKEY(client, channel)			(": 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
}

void User::leaveChannel(std::istringstream &request, std::string &client, Server &server) {
	std::string	target;
	if (!(request >> target)) // Error: no target
		return ;
	ChannelMap::iterator it = _channels.find(target);
	if (it == _channels.end()) // Error: not in channel
		return ;

	std::string msg;
	if (request >> msg)
		msg = RPL_PARTMESSAGE(client, target, msg);
	else
		msg = RPL_PART(client, target);
	it->second->leave(_nickname, msg);
	if (it->second->isEmpty()) {
		server.getChannels().erase(it->first);
		delete it->second;
		_channels.erase(it);
	}
	// #define ERR_NOTONCHANNEL(client, channel)			(": 442 " + client + " " + channel + " :You're not on that channel\r\n")
}

void User::kick(std::istringstream &request, std::string &client, Server &server) {
	(void) server;
	std::string channel;
	if (!(request >> channel)) // Error: no channel
		return ;
	ChannelMap::iterator it = _channels.find(channel);
	if (it == _channels.end()) // Error: not in channel
		return ;
	if (!it->second->isOperator(_nickname)) // Error: not operator
		return ;
	std::string target;
	if (!(request >> target)) // Error: no target
		return ;
	if (!it->second->isUser(target)) // Error: target not in channel
		return ;
	std::string msg;
	if (request >> msg) // No message: Error ???
		msg = RPL_KICK(client, channel, target, msg);
	else
		msg = RPL_KICK(client, channel, target, "");
	it->second->kick(target, msg);
}

void User::invite(std::istringstream &request, std::string &client, Server &server)
{
	(void) request;
	(void) client;
	(void) server;
	#define RPL_INVITERCVR(client, invitee, channel)	(":" + client + " INVITE " + invitee + " " + channel + "\r\n")
	#define RPL_INVITESNDR(client, invitee, channel)	(": 341 " + client + " " + invitee + " " + channel + "\r\n")
#define ERR_ALREADYREGISTRED(client)				(": 462 " + client + " ::Unauthorized command (already registered)\r\n")
}

void User::topic(std::istringstream &request, std::string &client, Server &server)
{
	(void) request;
	(void) client;
	(void) server;
	#define RPL_TOPIC(client, channel, topic)			(":" + client + " TOPIC " + channel + " :" + topic + "\r\n")
	#define RPL_NOTOPIC(client, channel)				(": 331 " + client + " " + channel + " :No topic is set\r\n")
	#define RPL_SEETOPIC(client, channel, topic)		(": 332 " + client + " " + channel + " :" + topic + "\r\n")

}

void User::mode(std::istringstream &request, std::string &client, Server &server)
{
	(void) request;
	(void) client;
	(void) server;
#define RPL_MODE(client, channel, mode, name)		(":" + client + " MODE " + channel + " " + mode + " " + name + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, modes) 	(": 324 " + client + " " + channel + " " + modes + "\r\n")
#define ERR_UNKNOWNMODE(client, mode)				(": 472 " + client + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_NOCHANMODES(channel)					(": 477 " + channel + " :Channel doesn't support modes\r\n")

}

void User::setByInvitation(bool byInvitation)
{
 (void) byInvitation;
}

// bool User::getByInvitation() const
// {

// }

void User::setPassword(std::string &password)
{
	(void) password;

}

void User::removePassword()
{

}
void User::giveOperatorStatus(std::string &user)
{
	(void) user;

}

void User::removeOperatorStatus(std::string &user)
{
	(void) user;

}

void User::setUserLimit(std::string &userLimit)
{
	(void) userLimit;

}

void User::removeUserLimit()
{

}

void User::checkPass(std::istringstream &request, std::string &client, Server &server) {
	(void) request;
	(void) client;
	(void) server;
}

void User::setUsername(std::istringstream &request, std::string &client, Server &server) {
	(void) client;
	(void) server;
	request >> _username;
}

void User::setNickname(std::istringstream &request, std::string &client, Server &server) {
	(void) client;
	(void) server;
	request >> _nickname;
}

std::string User::getUsername() const {return _username;}

std::string User::getNickname() const {return _nickname;}

sockaddr_in User::getAddr() const {return _addr;}

int User::getSockfd() const {return _sockfd;}