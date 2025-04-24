#include "User.hpp"
#include "ft_irc.hpp"

User::User(int servSockfd) {
	std::cout << "Waiting for connections..." << std::endl;
	socklen_t addr_len = sizeof(_addr);
	_sockfd = accept(servSockfd, (sockaddr *)&_addr, &addr_len); // on attend jusqu'a ce que le client se connecte
	if (_sockfd < 0)
		return ;

	std::cout << "Client connected: " << inet_ntoa(_addr.sin_addr) << ":" << ntohs(_addr.sin_port) << std::endl;

	// On envoie un message de bienvenue au client
	std::string welcome_msg = RPL_WELCOME(_nickname, CLIENT(_nickname, _username));
	send(_sockfd, welcome_msg.c_str(), welcome_msg.size(), 0);
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

void User::sendChannelMsg(std::istringstream &request) {
	std::string	token;

	if (request >> token) {
		Channel *tmpChannel = _channels.find(token)->second;
		if (!tmpChannel)
			return ;
		//tmpChannel->sendAllUser(_nickname, request);
	}
} // TODO

void User::sendPrivateMsg(std::istringstream &request) {
	(void)request;
} // TODO

void User::joinChannel(std::istringstream &request) {
	std::string	token;
	
	if (request >> token) {
		std::string	msg = RPL_JOIN(CLIENT(_nickname, _username), token);
		send(_sockfd, msg.c_str(), msg.size(), 0);
		std::cout << "Sended: " << msg << std::endl;
	}
}

void User::leaveChannel(std::istringstream &request)
{
	std::string	titleChannel;
	request >> titleChannel;

	ChannelMap::iterator it = this->_channels.find(titleChannel);
	if (it != _channels.end())
		it->second->kickUser(this->getNickname());
}

void User::kick(std::istringstream &request) // We must have greatest error handling
{
	std::string channel;
	std::string token;
	if (request >> channel) {
		request >> token;
		_channels.find(channel)->second->kickUser(token);
	}
}

void User::checkPass(std::istringstream &request) {(void)request;}

void User::setUsername(std::istringstream &request) {request >> _username;}

void User::setNickname(std::istringstream &request) {request >> _nickname;}

std::string User::getUsername() const {return _username;}

std::string User::getNickname() const {return _nickname;}

sockaddr_in User::getAddr() const {return _addr;}

int User::getSockfd() const {return _sockfd;}