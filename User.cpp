#include "User.hpp"
#include "ft_irc.hpp"

User::User() {}

User::User(int servSockfd) {
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
			if (it != commands.end()) {
				try {(this->*(it->second))(requestLine, client, server);
				} catch(const std::exception& e) {
					std::string msg = e.what();
					send(_sockfd, msg.c_str(), msg.size(), SOCK_NONBLOCK);
				}
			}
		}
		std::getline(request, tokenLine);
	}
}

void User::sendMsg(std::istringstream &request, std::string &client, Server &server) {
	std::string target;
	request >> target; // Error gerer par Hexchat

	std::string msg = request.str(); // PRIVMSG(client, target, "");
	if (target[0] == '#') {
		Channel *channel = server.getChannel(target);
		if (channel == NULL) // Error: no such channel
			throw std::runtime_error(ERR_NOSUCHCHANNEL(client, target));
		channel->sendAllUser(msg);
		std::cout << "Sended: " << msg << std::endl;
	} else {
		User *user = server.getUser(target);
		if (user == NULL) // Error: no such user
			throw std::runtime_error(ERR_NOSUCHNICK(client, target));
		send(user->getSockfd(), msg.c_str(), msg.size(), SOCK_NONBLOCK);
		std::cout << "Sended: " << msg << std::endl;
	}
}


void User::joinChannel(std::istringstream &request, std::string &client, Server &server) {
	std::string	channelName;
	request >> channelName; // Error gerer par Hexchat

	Channel *channel = server.getChannel(channelName);
	if (!channel) {
		channel = new Channel(channelName, this);
		_channels.insert(std::make_pair(channelName, channel));
		server.getChannels().insert(std::make_pair(channelName, channel));
	} else {
		if (channel->isByInvitation()/* && channel->isInvited(_nickname)*/) // Error: channel is invite only (stack d'attente ?)
			throw std::runtime_error(ERR_INVITEONLYCHAN(client, channelName));
		if (channel->isFull()) // Error: channel is full
			throw std::runtime_error(ERR_CHANNELISFULL(client, channelName));
		if (channel->isPassworded()) {
			std::string password;
			request >> password; // Error gerer par Hexchat
			if (password != channel->getPassword()) // Error: password incorrect
				throw std::runtime_error(ERR_BADCHANNELKEY(client, channelName));
		}
		channel->addUser(this);
		_channels.insert(std::make_pair(channelName, channel));
	}
	std::string	msg = RPL_JOIN(client, channelName);
	channel->sendAllUser(msg);
	std::cout << "Sended: " << msg << std::endl;
}

void User::leaveChannel(std::istringstream &request, std::string &client, Server &server) {
	std::string	channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	if (!channel->isUser(_nickname)) // Error: not in channel
		throw std::runtime_error(ERR_NOTONCHANNEL(client, channelName));

	std::string msg;
	if (request >> msg)
		msg = RPL_PARTMESSAGE(client, channelName, msg);
	else
		msg = RPL_PART(client, channelName);
	channel->leave(_nickname, msg);
	_channels.erase(channelName);
	if (channel->isEmpty()) {
		server.getChannels().erase(channelName);
		delete channel;
	}
}

void User::kick(std::istringstream &request, std::string &client, Server &server) {
	(void) server;
	std::string channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	if (!channel->isOperator(_nickname)) // Error: not operator
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client, channelName));
	std::string target;
	request >> target; // Error gerer par Hexchat
	if (!channel->isUser(target)) // Error: target not in channel
		throw std::runtime_error(ERR_USERNOTINCHANNEL(client, target, channelName));
	std::string msg;
	request >> msg; // Error gerer par Hexchat
	msg = RPL_KICK(client, channelName, target, msg);
	channel->kick(target);
	channel->sendAllUser(msg);
}

void	User::invite(std::istringstream &request, std::string &client, Server &server) {
	std::string targetName;
	request >> targetName; // Error gerer par Hexchat
	User *target = server.getUser(targetName);
	if (target == NULL) // Error: target not found
		throw std::runtime_error(ERR_NOSUCHNICK(client, targetName));
	std::string channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	if (!channel->isOperator(_nickname)) // Error: not operator
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client, channelName));
	if (channel->isUser(targetName)) // Error: target already in channel
		throw std::runtime_error(ERR_USERONCHANNEL(targetName, channelName));
	if (channel->isFull()) // Error: channel is full
		throw std::runtime_error(ERR_CHANNELISFULL(client, channelName));
	channel->addUser(target);
	std::string msg = RPL_INVITESNDR(client, targetName, channelName);
	send(target->getSockfd(), msg.c_str(), msg.size(), SOCK_NONBLOCK);
	msg = RPL_INVITERCVR(client, targetName, channelName);
	channel->sendAllUser(msg);
}

void User::topic(std::istringstream &request, std::string &client, Server &server) {
	(void) server;
	std::string channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));

	std::string msg;
	std::string topic;
	if (!(request >> topic)) {
		topic = channel->getTopic();
		if (topic.empty())
			msg = RPL_NOTOPIC(client, channelName);
		else
			msg = RPL_SEETOPIC(client, channelName, topic);
		send(_sockfd, msg.c_str(), msg.size(), SOCK_NONBLOCK);
		return ;
	}
	if (channel->isTopicDefRestricted() && !channel->isOperator(_nickname)) // Error: not operator
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client, channelName));
	channel->setTopic(topic);
	msg = RPL_TOPIC(client, channelName, topic);
	channel->sendAllUser(msg);
}

void User::mode(std::istringstream &request, std::string &client, Server &server) {
	std::string channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	std::string msg;
	std::string mode;
	if (!(request >> mode)) { // Liste des modes
		msg = RPL_CHANNELMODEIS(client, channelName, channel->getMode());
		send(_sockfd, msg.c_str(), msg.size(), SOCK_NONBLOCK);
		return ;
	}
	if (!channel->isOperator(_nickname)) // Error: not operator
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client, channelName));
	bool define = true;
	std::string token;
	for (std::string::iterator it = mode.begin(); it != mode.end(); it++) {
		try {
			if (*it == '+')
				define = true;
			else if (*it == '-')
				define = false;
			else if (*it == 'o')
				channel->setByInvitation(define);
			else if (*it == 'i')
				channel->setTopicRestriction(define);
			else if (*it == 'k') {
				token = "";
				if (define && !(request >> token)) // Error: no password (params) // is silent (standard RFC)
					continue;
				channel->setPassword(token);
			} else if (*it == 't') {
				if (!(request >> token)) // Error: no target (params) // is silent (standard RFC)
					continue;
				if (!channel->isUser(token)) // Error: target not in channel
					throw std::runtime_error(ERR_USERNOTINCHANNEL(client, token, channelName));
				channel->handleOperatorStatus(define, token);
			} else if (*it == 'l') {
				if (define && !(request >> token)) // Error: no user limit (params) // is silent (standard RFC)
					continue;
				channel->setUserLimit(!define, token);
			} else // Error: unknown mode
				throw std::runtime_error(ERR_UNKNOWNMODE(client, *it));
		} catch (std::exception &e) {
			msg = e.what();
			send(_sockfd, msg.c_str(), msg.size(), SOCK_NONBLOCK);
		}
		channel->setMode(mode);
		msg = RPL_MODE(client, channelName, mode, token); // TODO token -> tokens list
		channel->sendAllUser(msg);
	}
}

void User::checkPass(std::istringstream &request, std::string &client, Server &server) {
	std::string password;
	request >> password; // Error gerer par Hexchat
	if (password != server.getPassword()) // Error: password incorrect
		throw std::runtime_error(ERR_PASSWDMISMATCH(client));
}

void User::setUsername(std::istringstream &request, std::string &client, Server &server) {
	(void) client;
	(void) server;
	request >> _username; // Error gerer par Hexchat
}

void User::setNickname(std::istringstream &request, std::string &client, Server &server) {
	std::string nick;
	request >> nick; // Error gerer par Hexchat
	std::cout << "Set nickname: " << nick << std::endl;
	if (server.getUser(nick)) // Error: nickname already in use
		throw std::runtime_error(ERR_NICKNAMEINUSE(client, nick));
	_nickname = nick;
	(void) client; // bzr, on l'utilise [-Werror=unused-parameter]
}

std::string User::getUsername() const {return _username;}

std::string User::getNickname() const {return _nickname;}

sockaddr_in User::getAddr() const {return _addr;}

int User::getSockfd() const {return _sockfd;}