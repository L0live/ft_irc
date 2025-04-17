#include "Server.hpp"

Server::Server() : _password("") {init("6667");}

Server::Server(std::string port, std::string password) : _password(password) {init(port);}

Server::Server(const Server &src) {*this = src;}

Server  &Server::operator=(const Server &src) { // A revoir, copy profonde ?
	if (this == &src) {
		return *this;
	}
	_sockfd = src._sockfd;
	_serv_addr = src._serv_addr;
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
	memset(&hints, 0, sizeof(hints)); // '\0' partout
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP

	getaddrinfo("localhost", NULL, &hints, &res); // on prend les infos de notre host en local

	_serv_addr = *(sockaddr_in *)res->ai_addr;
	_serv_addr.sin_port = htons(atoi(port.c_str())); // la c notre port

	freeaddrinfo(res);
	bind(_sockfd, (struct sockaddr *)&_serv_addr, sizeof(_serv_addr)); // on relie le socket du server a notre host
}

void	Server::run() {
	std::cout << "Server running on " << inet_ntoa(_serv_addr.sin_addr) << ":" << ntohs(_serv_addr.sin_port) << std::endl;
	listen(_sockfd, 5); // on le met en mode ecoute

	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	std::cout << "Waiting for connections..." << std::endl;
	int client_sockfd = accept(_sockfd, (struct sockaddr *)&client_addr, &client_addr_len); // on attend jusqu'a ce qu'un client se connecte
	if (client_sockfd < 0) {
		throw std::runtime_error("Error: accepting connection");
	}

	std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

	// On envoie un message de bienvenue au client
	std::string welcome_msg = "Welcome to the IRC server!\n";
	if (send(client_sockfd, welcome_msg.c_str(), welcome_msg.size(), 0) < 0) {
		close(client_sockfd);
		throw std::runtime_error("Error: sending welcome message");
	}

	// on initialise le buffer
	char buffer[1024];
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		
		int bytes_received = recv(client_sockfd, buffer, sizeof(buffer) - 1, 0); // la on recoit le message du client
		if (bytes_received < 0) {
			close(client_sockfd);
			throw std::runtime_error("Error: receiving data");
		}
		buffer[bytes_received] = '\0';
		
		std::cout << "Received: " << buffer << std::endl;
	}
	close(client_sockfd);
}