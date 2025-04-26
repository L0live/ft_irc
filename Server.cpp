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
	for (ChannelMap::iterator it = _channels.begin(); it != _channels.end(); it++) {
		delete it->second;
	}
	for (UserMap::iterator it = _users.begin(); it != _users.end(); it++) {
		delete it->second;
	}
	
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

struct pollfd initPollfd(int sockfd) {
	struct pollfd pfd;
	pfd.fd = sockfd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	return pfd;
}

void	Server::run() {
	std::cout << "Server running on " << inet_ntoa(_addr.sin_addr) << ":" << ntohs(_addr.sin_port) << std::endl;
	listen(_sockfd, 5); // on le met en mode ecoute

	std::vector<struct pollfd> fds;
	fds.push_back(initPollfd(_sockfd));

/*
	On envoie un message de bienvenue au client
	std::string welcome_msg = RPL_WELCOME(_nickname, CLIENT(_nickname, _username));
	send(_sockfd, welcome_msg.c_str(), welcome_msg.size(), 0);
*/

	while (true) {
		if (poll(&fds[0], fds.size(), -1) <= 0)
			continue;
		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == _sockfd) {
					User *user = new User(_sockfd);
					if (user->getSockfd() == -1) {
						delete user;
						continue;
					}
					fds.push_back(initPollfd(user->getSockfd()));
					_users.insert(std::make_pair(user->getNickname(), user));
					_usersfd.insert(std::make_pair(user->getSockfd(), user));
				} else {
					User* user = _usersfd.find(fds[i].fd)->second;
					std::string tmp = user->receiveRequest();
					if (tmp.empty()) {
						_users.erase(user->getNickname());
						_usersfd.erase(fds[i].fd);
						fds.erase(fds.begin() + i);
						i--;
						delete user;
						continue;
					}
					std::istringstream request(tmp);
					user->interpretRequest(request, *this);
				}
			}
		}
	}
}

Server::CommandMap	Server::init_commands()
{

	CommandMap commands;

	commands["PRIVMSG"] = &User::sendMsg;
	commands["JOIN"] = &User::joinChannel;
	commands["PART"] = &User::leaveChannel;
	commands["KICK"] = &User::leaveChannel;
	commands["INVITE"] = &User::invite;
	commands["TOPIC"] = &User::topic;
	commands["MODE"] = &User::mode;
	// commands["WHO"] = &User::who; // liste tous les users du channel
	commands["NICK"] = &User::setNickname;
	commands["USER"] = &User::setUsername;
	commands["PASS"] = &User::checkPass;
	return commands;
}

ChannelMap	&Server::getChannels() {return _channels;}

UserMap	&Server::getUsers() {return _users;}