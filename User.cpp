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

	// On envoie un message de bienvenue au client
	std::string welcome_msg = "Welcome to the ft_IRC server!\n";
	send(_sockfd, welcome_msg.c_str(), welcome_msg.size(), 0);
}

User::~User() {
	if (_sockfd != -1) {
		close(_sockfd);
	}
}

void User::sendChannelMsg(std::string &channel, std::string &msg) {
	Channel *tmpChannel = _channels.find(channel)->second;
	if (!tmpChannel)
		return ;
	tmpChannel->sendAllUser(_nickname, msg);
}

void User::sendPrivateMsg(std::string &msg, std::string &target) {
	(void)msg;
	(void)target;
} // TODO

void User::joinChannel(std::string &channel) {
	(void)channel;
} // TODO

void User::leaveChannel() {} // TODO

void User::setUsername(std::string &username) {_username = username;}

void User::setNickname(std::string &nickname) {_nickname = nickname;}

std::string User::getUsername() const {return _username;}

std::string User::getNickname() const {return _nickname;}

sockaddr_in User::getAddr() const {return _addr;}

int User::getSockfd() const {return _sockfd;}