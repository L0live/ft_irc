#include "../include/User.hpp"
#include "../include/ft_irc.hpp"

User::User() {}

User::User(int servSockfd) : _saveBuffer(false), _register(false){
	socklen_t addr_len = sizeof(_addr);
	_nickValid = false;
	_userValid = false;
	_passValid = UNSET;
	_sockfd = accept(servSockfd, (sockaddr *)&_addr, &addr_len); // on attend jusqu'a ce que le client se connecte
	if (_sockfd < 0)
		return ;
	fcntl(_sockfd, F_SETFL, O_NONBLOCK);
	std::cout << "Client connected: " << inet_ntoa(_addr.sin_addr) << ":" << ntohs(_addr.sin_port) << std::endl;
}

User::~User() {
	if (_sockfd != -1) {
		close(_sockfd);
	}
}

void User::receiveRequest() {
	char	tmpBuffer[1024];
	ssize_t bytes;

	if (!_saveBuffer)
		_buffer.clear();
	do {
		memset(tmpBuffer, '\0', sizeof(tmpBuffer));
		bytes = recv(_sockfd, tmpBuffer, sizeof(tmpBuffer) - 1, 0);
		_buffer += tmpBuffer;
	} while (bytes == 1023);
	if (std::string(tmpBuffer).empty())
		_buffer.clear();
	else if (_buffer[_buffer.size() - 1] != '\n')
		_saveBuffer = true;
	else
		_saveBuffer = false;
	std::cout << "Received: " << _buffer << std::endl;
}

void	User::sendRequest(std::string msg) {
	std::cout << "Sended: " << msg << std::endl;
	send(_sockfd, msg.c_str(), msg.size(), MSG_NOSIGNAL);
}

void User::interpretRequest(Server &server) {
	if (_buffer.empty())
		_buffer = "QUIT :Leaving with ctrl+C\r\n";
	if (_saveBuffer)
		return ;
	static Server::CommandMap commands = server.init_commands();
	std::istringstream request(_buffer);
	std::string client = CLIENT(_nickname, _username);

	std::string	tokenLine;
	while (std::getline(request, tokenLine)) {
		std::istringstream	requestLine(tokenLine);
		std::string	token;
		if (requestLine >> token) {
			Server::CommandMap::iterator	it = commands.find(token);
			try {
				if (it == commands.end())
					throw std::runtime_error(ERR_UNKNOWNCOMMAND(client, token));
				if (!_register && token != "PASS" && token != "NICK" && token != "USER" && token != "QUIT")
					throw std::runtime_error(ERR_NOTREGISTERED());
				(this->*(it->second))(requestLine, client, server);
			} catch(const std::exception& e) {
				sendRequest(e.what());
			}
		}
	}
}

void User::sendMsg(std::istringstream &request, std::string &client, Server &server) {
	std::string target;
    if (!(request >> target))
		throw std::runtime_error(ERR_NORECIPIENT(_nickname, "PRIVMSG"));

	std::string token;
	std::string msg;
	request >> token;
	if (token.empty())
        throw std::runtime_error(ERR_NOTEXTTOSEND(_nickname));

	if (token[0] == ':')
		token.erase(token.begin());
	do { msg += token + " ";} while (request >> token);

	if (target[0] == '#') {
		Channel *channel = server.getChannel(target);
		if (channel == NULL) // Error: no such channel
			throw std::runtime_error(ERR_NOSUCHCHANNEL(_nickname, target));
		if (!channel->isUser(_nickname)) // Error: not in channel
			throw std::runtime_error(ERR_CANNOTSENDTOCHAN(_nickname, target));
		channel->sendAllUser(PRIVMSG(client, target, msg), &_nickname);
	} else {
		User *user = server.getUser(target);
		if (user == NULL) // Error: no such user
			throw std::runtime_error(ERR_NOSUCHNICK(_nickname, target));
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
	if (!(request >> channelName))
		throw	std::runtime_error(ERR_NEEDMOREPARAMS(client, "JOIN"));

	if (channelName[0] != '#')
		channelName = "#" + channelName;
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

	if (!(request >> channelName))
		throw	std::runtime_error(ERR_NEEDMOREPARAMS(client, "PART"));

	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	if (!channel->isUser(_nickname)) // Error: not in channel
		throw std::runtime_error(ERR_NOTONCHANNEL(client, channelName));

	std::string msg;
	if (request >> msg)
	{
		if (msg[0] == ':')
			msg.erase(msg.begin());
		std::string token;
		request >> token;
		do { msg += token + " ";} while (request >> token);	
		msg = RPL_PARTMESSAGE(client, channelName, msg);
	}
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
	if (!(request >> channelName))
		throw	std::runtime_error(ERR_NEEDMOREPARAMS(client, "KICK"));

	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	if (!channel->isUser(_nickname)) // Error: _nickname not in channel
		throw std::runtime_error(ERR_NOTONCHANNEL(client, channelName));		
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
	if (!(request >> targetName))
		throw	std::runtime_error(ERR_NEEDMOREPARAMS(client, "INVITE"));
	
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
	if (!(request >> channelName))
		throw	std::runtime_error(ERR_NEEDMOREPARAMS(client, "TOPIC"));
	
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	if (!channel->isUser(_nickname)) // Error: _nickname not in channel
		throw std::runtime_error(ERR_NOTONCHANNEL(client, channelName));
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
	if (!(request >> channelName))
		throw	std::runtime_error(ERR_NEEDMOREPARAMS(client, "MODE"));
	
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	std::string msg;
	std::string mode;
	if (!(request >> mode)) // Mode list // Not an error
		throw std::runtime_error(RPL_CHANNELMODEIS(client, channelName, channel->getMode()));
	if (!channel->isUser(_nickname)) // Error: _nickname not in channel
		throw std::runtime_error(ERR_NOTONCHANNEL(client, channelName));		
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
	if (!(request >> channelName))
		throw	std::runtime_error(ERR_NEEDMOREPARAMS(client, "WHO"));
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) // Error: no such channel
		throw std::runtime_error(ERR_NOSUCHCHANNEL(client, channelName));
	sendRequest(RPL_NAMEREPLY(client, channelName, channel->who()));
	sendRequest(RPL_ENDOFNAMES(client, channelName));
}

void User::checkPass(std::istringstream &request, std::string &client, Server &server) {
	(void) client;
	std::string password;
	request >> password; // Error gerer par Hexchat

	if (server.getPassword().empty() || password == server.getPassword()) // Error: password incorrect
		_passValid = TRUE;
	else
		_passValid = FALSE;
	checkRegister(client, server);
}

bool User::checkName(std::string name) {
	if ( name.empty() || name.length() > 9 || !std::isalpha(name[0]))
		return false;
	const std::string allowedSpecial = "-[]\\`^_{}|";
	for (size_t i = 1; i < name.length(); i++) {
		if (!std::isalpha(name[i]) && !std::isdigit(name[i])
			&& allowedSpecial.find(name[i]) == std::string::npos)
			return false;		
	}
	return true;
}

void User::checkRegister(std::string &client, Server &server)
{
	(void) client;
	if (server.getPassword().empty())
		_passValid = TRUE;	
	if (!_register && _nickValid && _userValid && !_passValid && !server.getPassword().empty())
		throw std::runtime_error(ERR_PASSWDMISMATCH(_nickname));
	if(!_register && _passValid == TRUE && _userValid && _nickValid) {
		sendRequest(RPL_WELCOME(_nickname, CLIENT(_nickname, _username)));
		server.getUsers().insert(std::make_pair(_nickname, this));
		_register = true;
	}
	else if (_register)
		throw std::runtime_error(ERR_ALREADYREGISTRED(client));
}

void User::setUsername(std::istringstream &request, std::string &client, Server &server) {
	std::string	user;
	if (!(request >> user))
		throw	std::runtime_error(ERR_NEEDMOREPARAMS(client, "USER"));
	if (!checkName(user))
		throw std::runtime_error(ERR_ERRONEUSUSERNAME(user, user));
	_username = user;
	_userValid = true;
	checkRegister(client, server);
}

void User::setNickname(std::istringstream &request, std::string &client, Server &server) {
	std::string nick;
	if (!(request >> nick))
	{
		if (_nickname.empty())
			throw	std::runtime_error(ERR_NONICKNAMEGIVEN((std::string)"", "*"));
		throw	std::runtime_error(ERR_NONICKNAMEGIVEN(client, _nickname));
	}
	if (!checkName(nick))
		throw std::runtime_error(ERR_ERRONEUSNICKNAME(client, nick));
	if (server.getUser(nick)) // Error: nickname already in use
		throw std::runtime_error(ERR_NICKNAMEINUSE(nick));
	_nickname = nick;
	_nickValid = true;
	checkRegister(client, server);
}

std::string User::getUsername() const {return _username;}

std::string User::getNickname() const {return _nickname;}

sockaddr_in User::getAddr() const {return _addr;}

int User::getSockfd() const {return _sockfd;}

bool User::getRegistrationState() const {return _register;}
