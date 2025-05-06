#include "../include/User.hpp"
#include "../include/ft_irc.hpp"

User::User() {}

User::User(int servSockfd) {
	socklen_t addr_len = sizeof(_addr);
	_sockfd = accept(servSockfd, (sockaddr *)&_addr, &addr_len); // on attend jusqu'a ce que le client se connecte
	if (_sockfd < 0)
		return ;
	fcntl(_sockfd, F_SETFL, O_NONBLOCK);
	std::cout << "Client connected: " << inet_ntoa(_addr.sin_addr) << ":" << ntohs(_addr.sin_port) << std::endl;
	_registrationState = PASS;
}

User::~User() {
	if (_sockfd != -1) {
		close(_sockfd);
	}
}

std::string User::receiveRequest() {
	std::string buffer;
	char	tmpBuffer[1024];

	memset(tmpBuffer, '\0', sizeof(tmpBuffer));
	while (recv(_sockfd, tmpBuffer, sizeof(tmpBuffer) - 1, 0) > 0) { // TODO gerer le Ctrl+D (EOF)
		buffer += tmpBuffer;
		memset(tmpBuffer, '\0', sizeof(tmpBuffer));
	}
	std::cout << std::endl;
	std::cout << "Received: " << buffer << std::endl;
	return buffer;
}

void	User::sendRequest(std::string msg) {
	std::cout << "Sended: " << msg << std::endl;
	send(_sockfd, msg.c_str(), msg.size(), MSG_NOSIGNAL);
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
			try {
				if (it == commands.end())
					throw std::runtime_error(ERR_UNKNOWNCOMMAND(client, token));
				if (token != "PASS" && token != "NICK" && token != "USER" && _registrationState != REGISTER)
					throw std::runtime_error(ERR_NOTREGISTERED());
				(this->*(it->second))(requestLine, client, server);
			} catch(const std::exception& e) {
				sendRequest(e.what());
			}
		}
		std::getline(request, tokenLine);
	}
}

void User::sendMsg(std::istringstream &request, std::string &client, Server &server) {
	std::string target;
	request >> target; // Error gerer par Hexchat

	std::string token;
	std::string msg;
	request >> token;

	if (token[0] == ':')
		token.erase(token.begin());
	do { msg += token + " ";} while (request >> token);
	if (target[0] == '#') {
		Channel *channel = server.getChannel(target);
		if (channel == NULL) // Error: no such channel
			throw std::runtime_error(ERR_NOSUCHCHANNEL(client, target));
		if (!channel->isUser(_nickname)) // Error: not in channel
			throw std::runtime_error(ERR_NOTONCHANNEL(client, target));
		channel->sendAllUser(PRIVMSG(client, target, msg), &_nickname);
	} else {
		User *user = server.getUser(target);
		if (user == NULL) // Error: no such user
			throw std::runtime_error(ERR_NOSUCHNICK(client, target));
		user->sendRequest(PRIVMSG(client, target, msg));
	}
}

void User::quit(std::istringstream &request, std::string &client, Server &server) {
	std::string msg;
	std::string token;

	request >> token;
	if (token[0] == ':')
		token.erase(token.begin());
	do { msg += token + " ";} while (request >> token);
	msg = RPL_QUIT(client, msg);
	for (ChannelMap::iterator it = _channels.begin(); it != _channels.end(); it++) {
		Channel *channel = it->second;
		channel->leave(_nickname, msg);
		if (channel->isEmpty()) {
			server.getChannels().erase(it->first);
			delete channel;
		}
	}
	server.toLeave();
}

void User::joinChannel(std::istringstream &request, std::string &client, Server &server) {
	std::string	channelName;
	request >> channelName; // Error gerer par Hexchat
	if (channelName[0] != '#')
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	Channel *channel = server.getChannel(channelName);
	if (!channel) {
		channel = new Channel(channelName, this);
		_channels.insert(std::make_pair(channelName, channel));
		server.getChannels().insert(std::make_pair(channelName, channel));
	} else {
		if (channel->isUser(_nickname)) // Error: already in channel
			throw std::runtime_error(ERR_USERONCHANNEL(client, channelName));
		if (channel->isByInvitation() && !channel->isInvited(_nickname)) // Error: channel is invite only
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
	channel->sendAllUser(msg, NULL);
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
	channel->sendAllUser(msg, NULL);
	channel->kick(target);
	if (channel->isEmpty()) {
		server.getChannels().erase(channelName);
		delete channel;
	}
}

void	User::invite(std::istringstream &request, std::string &client, Server &server) {
	std::string targetName;
	request >> targetName; // Error gerer par Hexchat
	User *target = server.getUser(targetName);
	if (!target) // Error: target not found
		throw std::runtime_error(ERR_NOSUCHNICK(client, targetName));
	std::string channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (!channel) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	if (!channel->isOperator(_nickname)) // Error: not operator
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client, channelName));
	if (channel->isUser(targetName)) // Error: target already in channel
		throw std::runtime_error(ERR_USERONCHANNEL(targetName, channelName));
	if (channel->isFull()) // Error: channel is full
		throw std::runtime_error(ERR_CHANNELISFULL(client, channelName));
	channel->invite(targetName);
	target->sendRequest(RPL_INVITESNDR(client, targetName, channelName));
	channel->sendAllUser(RPL_INVITERCVR(client, targetName, channelName), NULL);
}

void User::topic(std::istringstream &request, std::string &client, Server &server) {
	(void) server;
	std::string channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));

	std::string topic;
	if (!(request >> topic)) {
		topic = channel->getTopic();
		if (topic.empty())
			sendRequest(RPL_NOTOPIC(client, channelName));
		else
			sendRequest(RPL_SEETOPIC(client, channelName, topic));
		return ;
	}
	if (channel->isTopicDefRestricted() && !channel->isOperator(_nickname)) // Error: not operator
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client, channelName));
	std::string token;
	if (topic[0] == ':')
		topic.erase(topic.begin());
	while (request >> token)
		topic += token + " ";
	channel->setTopic(topic);
	channel->sendAllUser(RPL_TOPIC(client, channelName, topic), NULL);
}

void sendModeFormatter(std::string &mode, std::string &tokens) {
	if (!tokens.empty()) {
		tokens.erase(tokens.end() - 1);
		size_t pos = tokens.rfind(' ');
		if (pos == std::string::npos)
			pos = 0;
		else
			pos += 1;
		tokens.insert(tokens.begin() + pos, 1, ':');
	}
	for (std::string::iterator it = mode.begin() + 1; it != mode.end(); it++) {
		if (*it == *(it - 1) && (*it == '+' || *it == '-'))
			mode.erase(it);
	}
}

void User::mode(std::istringstream &request, std::string &client, Server &server) {
	std::string channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	std::string msg;
	std::string mode;
	if (!(request >> mode)) // Mode list // Not an error
		throw std::runtime_error(RPL_CHANNELMODEIS(client, channelName, channel->getMode()));
	if (!channel->isOperator(_nickname)) // Error: not operator
		throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client, channelName));
	bool sendMode = false;
	int define = 3;
	std::string tokens;
	for (std::string::iterator it = mode.begin(); it != mode.end(); it++) {
		std::string token;
		bool changed = false;
		try {
			if (*it == '+')
				define = true;
			else if (*it == '-')
				define = false;
			else if (*it == 'i')
				channel->setByInvitation(define, &changed);
			else if (*it == 't')
				channel->setTopicRestriction(define, &changed);
			else if (*it == 'k') {
				if (define && !(request >> token)) // Error: no password (params) // is silent (standard RFC)
					continue;
				channel->setPassword(token, &changed);
			} else if (*it == 'o') {
				if (!(request >> token)) // Error: no target (params) // is silent (standard RFC)
					continue;
				if (!channel->isUser(token)) // Error: target not in channel
					throw std::runtime_error(ERR_USERNOTINCHANNEL(client, token, channelName));
				channel->handleOperatorStatus(define, token, &changed);
			} else if (*it == 'l') {
				if (define && !(request >> token)) // Error: no user limit (params) // is silent (standard RFC)
					continue;
				channel->setUserLimit(!define, token, &changed);
			} else // Error: unknown mode
				throw std::runtime_error(ERR_UNKNOWNMODE(client, *it));
		} catch (std::exception &e) {
			sendRequest(e.what());
		}
		if (*it != '+' && *it != '-' && !changed) {
			it = mode.erase(it);
			if (it == mode.end())
				break;
		} else {
			if (*it == 'o' || (define && (*it == 'k' || *it == 'l')))
				tokens += token + " ";
			if (define == 3)
				mode.insert(mode.begin(), 1, '+');
			sendMode = true;
		}
	}
	if (sendMode) {
		sendModeFormatter(mode, tokens);
		channel->sendAllUser(RPL_MODE(client, channelName, mode, tokens), NULL);
	}
}

void User::who(std::istringstream &request, std::string &client, Server &server) {
	std::string channelName;
	request >> channelName; // Error gerer par Hexchat
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	sendRequest(RPL_NAMEREPLY(_nickname, channelName, channel->who()));
	sendRequest(RPL_ENDOFNAMES(_nickname, channelName));
}

void User::checkPass(std::istringstream &request, std::string &client, Server &server) {
	(void) client;
	std::string password;
	request >> password; // Error gerer par Hexchat
	if (server.getPassword().empty() || password == server.getPassword()) // Error: password incorrect
		_registrationState = NICK;
}

void User::setUsername(std::istringstream &request, std::string &client, Server &server) {
	(void) client;
	request >> _username; // Error gerer par Hexchat
	if (_registrationState == PASS && !server.getPassword().empty())
		throw std::runtime_error(ERR_PASSWDMISMATCH(CLIENT(_nickname, _username)));
	if (_registrationState == USER) {
		sendRequest(RPL_WELCOME(_nickname, CLIENT(_nickname, _username)));
		server.getUsers().insert(std::make_pair(_nickname, this));
		_registrationState = REGISTER;
	}
}

void User::setNickname(std::istringstream &request, std::string &client, Server &server) {
	std::string nick;
	request >> nick; // Error gerer par Hexchat
	if (server.getUser(nick)) // Error: nickname already in use
		throw std::runtime_error(ERR_NICKNAMEINUSE(nick));
	_nickname = nick;
	if (_registrationState == NICK || server.getPassword().empty())
		_registrationState = USER;
	if (_registrationState == USER && !_username.empty()) {
		sendRequest(RPL_WELCOME(_nickname, CLIENT(_nickname, _username)));
		server.getUsers().insert(std::make_pair(_nickname, this));
		_registrationState = REGISTER;
	}
	(void) client; // bzr, on l'utilise [-Werror=unused-parameter]
}

std::string User::getUsername() const {return _username;}

std::string User::getNickname() const {return _nickname;}

sockaddr_in User::getAddr() const {return _addr;}

int User::getSockfd() const {return _sockfd;}