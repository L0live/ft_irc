#include "User.hpp"
#include "ft_irc.hpp"

User::User(int servSockfd) {
	std::cout << "Waiting for connections..." << std::endl;
	socklen_t addr_len = sizeof(_addr);
	_sockfd = accept(servSockfd, (sockaddr *)&_addr, &addr_len); // on attend jusqu'a ce que le client se connecte
	if (_sockfd < 0) {
		throw std::runtime_error("Error: accepting connection");
	}

	std::cout << "Client connected: " << inet_ntoa(_addr.sin_addr) << ":" << ntohs(_addr.sin_port) << std::endl;

	receiveRequest();
	std::istringstream	authRequest(receiveRequest());
	interpretRequest(authRequest);

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
	std::string	tmp;
	char buffer[1024];
	memset(buffer, '\0', sizeof(buffer));
	while (recv(_sockfd, buffer, sizeof(buffer) - 1, 0)) {
		tmp += buffer;
		memset(buffer, '\0', sizeof(buffer));
	}

	std::cout << "Received: " << tmp << std::endl;
	return tmp;
}

User::CommandMap	User::init_commands() {

	CommandMap commands;

	commands["PASS"] = &User::checkPass;
	commands["NICK"] = &User::setNickname;
	commands["USER"] = &User::setUsername;
	commands["JOIN"] = &User::joinChannel;
	return commands;
}

void User::interpretRequest(std::istringstream &request) {
	static CommandMap commands = init_commands();

	std::string	token;
	if (request >> token) {
		std::cout << "command: " << token << std::endl;
		CommandMap::iterator	it = commands.find(token);
		if (it != commands.end())
			(this->*(it->second))(request);
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

void User::joinChannel(std::istringstream &channel) {
	std::string	token;
	
	if (channel >> token)
		std::cout << _nickname << " has joined " << token << std::endl;
} // TODO

void User::leaveChannel(std::istringstream &request) {(void)request;} // TODO

void User::checkPass(std::istringstream &request) {(void)request;}

void User::setUsername(std::istringstream &request) {request >> _username;}

void User::setNickname(std::istringstream &request) {request >> _nickname;}

std::string User::getUsername() const {return _username;}

std::string User::getNickname() const {return _nickname;}

sockaddr_in User::getAddr() const {return _addr;}

int User::getSockfd() const {return _sockfd;}