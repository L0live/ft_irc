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
	if (tmp <= 0) // si empty modifié pour renvoyer uniquement chaine vide
		return ("");	
	std::cout << "Received: " << buffer << std::endl;
	return std::string(buffer);
}

void User::interpretRequest(std::istringstream &request, Server &server) {
	static Server::CommandMap commands = server.init_commands();

	std::string	tokenLine;
	std::getline(request, tokenLine);
	while (!tokenLine.empty()) {
		std::istringstream	requestLine(tokenLine);
		std::string	token;
		if (requestLine >> token) {
			Server::CommandMap::iterator	it = commands.find(token);
			if (it != commands.end())
				(this->*(it->second))(requestLine);
		}
		std::getline(request, tokenLine);
	}
}

void User::sendMsg(std::istringstream &request, std::string &client, Server &server){
	//PRIVMSG(client, target, message);
	std::string token;
	if (request >> token) {
		Channel *tmpChannel = _channels.find(token)->second;
		if (!tmpChannel)
			return ;
		
		//tmpChannel->sendAllUser(_nickname, request);
	}
} // TODO


void User::joinChannel(std::istringstream &request, std::string &client, Server &server) {
	std::string	token;
	
	if (request >> token) {
		std::string	msg = RPL_JOIN(CLIENT(_nickname, _username), token);
		send(_sockfd, msg.c_str(), msg.size(), 0);
		std::cout << "Sended: " << msg << std::endl;
	}
#define ERR_CHANNELISFULL(client, channel)			(": 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(client, channel)			(": 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(client, channel)			(": 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
}

void User::leaveChannel(std::istringstream &request, std::string &client, Server &server)
{
	std::string	titleChannel;
	request >> titleChannel;

	ChannelMap::iterator it = this->_channels.find(titleChannel);
	if (it != _channels.end())
		it->second->Leave(this->getNickname());
}

void User::kick(std::istringstream &request, std::string &client, Server &server) // We must have greatest error handling
{
	std::string channel;
	std::string token;
	if (request >> channel) {
		request >> token;
		_channels.find(channel)->second->kickUser(token);
	}
}

void User::invite(std::istringstream &request, std::string &client, Server &server)
{
	#define RPL_INVITERCVR(client, invitee, channel)	(":" + client + " INVITE " + invitee + " " + channel + "\r\n")
	#define RPL_INVITESNDR(client, invitee, channel)	(": 341 " + client + " " + invitee + " " + channel + "\r\n")
#define ERR_ALREADYREGISTRED(client)				(": 462 " + client + " ::Unauthorized command (already registered)\r\n")
}

void User::topic(std::istringstream &request, std::string &client, Server &server)
{
	#define RPL_TOPIC(client, channel, topic)			(":" + client + " TOPIC " + channel + " :" + topic + "\r\n")
	#define RPL_NOTOPIC(client, channel)				(": 331 " + client + " " + channel + " :No topic is set\r\n")
	#define RPL_SEETOPIC(client, channel, topic)		(": 332 " + client + " " + channel + " :" + topic + "\r\n")

}

void User::mode(std::istringstream &request, std::string &client, Server &server)
{
#define RPL_MODE(client, channel, mode, name)		(":" + client + " MODE " + channel + " " + mode + " " + name + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, modes) 	(": 324 " + client + " " + channel + " " + modes + "\r\n")
#define ERR_UNKNOWNMODE(client, mode)				(": 472 " + client + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_NOCHANMODES(channel)					(": 477 " + channel + " :Channel doesn't support modes\r\n")

}

void User::setByInvitation(bool byInvitation)
{
 (void) byInvitation;
}

bool User::getByInvitation() const
{

}

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

void User::setUserLimit(std::string &userLimit)
{
	(void) userLimit;

}

void User::removeUserLimit()
{

}

void User::checkPass(std::istringstream &request) {(void)request;}

void User::setUsername(std::istringstream &request) {request >> _username;}

void User::setNickname(std::istringstream &request) {request >> _nickname;}

std::string User::getUsername() const {return _username;}

std::string User::getNickname() const {return _nickname;}

sockaddr_in User::getAddr() const {return _addr;}

int User::getSockfd() const {return _sockfd;}