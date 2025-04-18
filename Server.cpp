#include "Server.hpp"
#include "ft_irc.hpp"

Server::Server() : _password("") {init("6667");}

Server::Server(std::string port, std::string password) : _password(password) {init(port);}

Server::Server(const Server &src) {*this = src;}

Server  &Server::operator=(const Server &src) { // A revoir, copy profonde ?
	if (this == &src) {
		return *this;
	}
	_sockfd = src._sockfd;
	_addr = src._addr;
	_password = src._password;
	_channels = src._channels;
	_users = src._users;
	return *this;
}

Server::~Server() {
	// for (ChannelMap::iterator it = _channels.begin(); it != _channels.end(); it++) {
	// 	delete it->second;
	// }
	// for (std::map<std::string, IClient *>::iterator it = _users.begin(); it != _users.end(); it++) {
	// 	delete it->second;
	// }
	if (_sockfd != -1) {
		close(_sockfd);
	}
}

void	handleSIGINT(int) {throw std::runtime_error("\nServer shutting down...");}

void	Server::init(std::string port) {

	signal(SIGINT, handleSIGINT); // On gère le signal SIGINT (Ctrl+C)

	// Creation du socket du server
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		throw std::runtime_error("Error: creating socket");
	}

	// setsockopt() ???

	// On initialise des trucs
	addrinfo hints, *res;
	memset(&hints, '\0', sizeof(hints)); // '\0' partout
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP

	getaddrinfo("localhost", NULL, &hints, &res); // on prend les infos de notre host en local

	_addr = *(sockaddr_in *)res->ai_addr;
	_addr.sin_port = htons(atoi(port.c_str())); // la c notre port

	freeaddrinfo(res);
	bind(_sockfd, (struct sockaddr *)&_addr, sizeof(_addr)); // on relie le socket du server a notre host
}

void	Server::run() {
	std::cout << "Server running on " << inet_ntoa(_addr.sin_addr) << ":" << ntohs(_addr.sin_port) << std::endl;
	listen(_sockfd, 5); // on le met en mode ecoute

	User user(_sockfd); // on cree un user

	// on initialise le buffer
	char buffer[1024];
	while (true) {
		memset(buffer, '\0', sizeof(buffer));

		recv(user.getSockfd(), buffer, sizeof(buffer) - 1, 0); // la on recoit le message du client
		std::cout << "Received: " << buffer << std::endl;
		if (!strncmp(buffer, "JOIN", 3))
			break;
	}
}