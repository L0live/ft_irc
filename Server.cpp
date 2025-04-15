#include "Server.hpp"
#include <string.h>

Server::Server() : _port("6667"), _password("") {}

Server::Server(std::string port, std::string password) : _port(port), _password(password) {}

Server::Server(const Server &src) {*this = src;}

Server  &Server::operator=(const Server &src) {
	_port = src._port;
	_password = src._password;
	_channels = src._channels;
	_users = src._users;
	return *this;
}

Server::~Server() {
	// for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
	// 	delete it->second;
	// }
	// for (std::map<std::string, IClient *>::iterator it = _users.begin(); it != _users.end(); it++) {
	// 	delete it->second;
	// }
}

void	Server::run() {
	// Creation du socket du server
	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		throw std::runtime_error("Error: creating socket");
	}

	// setsockopt() ???

	// On initialise des trucs
	addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints)); // '\0' partout
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP

	getaddrinfo("localhost", NULL, &hints, &res); // on prend les infos de notre host en local

	sockaddr_in serv_addr = *(sockaddr_in *)res->ai_addr;
	serv_addr.sin_port = htons(atoi(_port.c_str())); // la c notre port

	freeaddrinfo(res);
	std::cout << "Server running on " << inet_ntoa(serv_addr.sin_addr) << ":" << ntohs(serv_addr.sin_port) << std::endl;
	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); // on relie le socket du server a notre host

	listen(sockfd, 5); // on le met en mode ecoute

	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	std::cout << "Waiting for connections..." << std::endl;
	int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len); // on attend jusqu'a ce qu'un client se connecte
	if (client_sockfd < 0) {
		close(sockfd);
		throw std::runtime_error("Error: accepting connection");
	}

	std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

	// on initialise le buffer
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	int bytes_received = recv(client_sockfd, buffer, sizeof(buffer) - 1, 0); // la on recoit le message du client
	if (bytes_received < 0) {
		close(client_sockfd);
		close(sockfd);
		throw std::runtime_error("Error: receiving data");
	}
	buffer[bytes_received] = '\0';

	std::cout << "Received: " << buffer << std::endl;
	close(client_sockfd); // et biensur on close tout
	close(sockfd);
}