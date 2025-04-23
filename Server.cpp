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
	_usersfd = src._usersfd;
	return *this;
}

Server::~Server() {
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

	int opt = 1;
	setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
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

	std::vector<struct pollfd> fds;
	struct pollfd server_poll;
    server_poll.fd = _sockfd;
    server_poll.events = POLLIN;
	fds.push_back(server_poll);

	while (true) {
		int ret = poll(&fds[0], fds.size(), 1);
		if (ret <= 0)
			continue;
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN){
				if (fds[i].fd == _sockfd)
				{
					//nouvelle connexion d'user
					User *user = new User(_sockfd);
					struct pollfd client_poll;
				    client_poll.fd = user->getSockfd();
				    client_poll.events = POLLIN;
					fds.push_back(client_poll);
					this->_users.insert(std::make_pair(user->getNickname(), user));
					this->_usersfd.insert(std::make_pair(user->getSockfd(), user));
				}
				else
				{
					std::map<int, User*>::iterator it = _usersfd.find(fds[i].fd);
					if (it != _usersfd.end()) //user find
					{
						User* tmp = it->second;
						std::string stdTmp = tmp->receiveRequest();
						if (stdTmp.empty())
						{
							// Del user, userfd et iterator
							_users.erase(tmp->getNickname());
							_usersfd.erase(it);
							fds.erase(fds.begin() + i);
							i--;
							delete (tmp);
							continue;
						}		
						std::istringstream request(stdTmp);
						tmp->interpretRequest(request);
					}
				}

			}
		}
	}
}

