#include "../include/Channel.hpp"
#include "../include/ft_irc.hpp"

Channel::Channel() {}

Channel::Channel(std::string &name, User *user)
: _name(name), _topicRestriction(true), _userLimit(-1), _byInvitation(false) {
	_operators.insert(std::make_pair(user->getNickname(), user));
}

Channel::Channel(const Channel &src) {*this = src;}

Channel &Channel::operator=(const Channel &src) {
	if (this == &src)
		return *this;
	_name = src._name;
	_password = src._password;
	_topic = src._topic;
	_topicRestriction = src._topicRestriction;
	_users = src._users;
	_operators = src._operators;
	_userLimit = src._userLimit;
	_byInvitation = src._byInvitation;
	return *this;
}

Channel::~Channel() {}

void	Channel::sendAllUser(std::string msg, std::string *nick) {
	for (UserMap::iterator it = _users.begin(); it != _users.end(); ++it) {
		if (nick && *nick == it->first)
			continue;
		it->second->sendRequest(msg);
	}
	for (UserMap::iterator it = _operators.begin(); it != _operators.end(); it++) {
		if (nick && *nick == it->first)
			continue;
		it->second->sendRequest(msg);
	}
	(void) nick;
}

std::string Channel::who() {
	std::string	nicknames;
	for (UserMap::iterator it = _operators.begin() ; it != _operators.end(); it++)
	{
		nicknames += "@" + it->first + " ";
	}
	for (UserMap::iterator it = _users.begin() ; it != _users.end(); it++)
	{
		nicknames += it->first + " ";
	}
	return (nicknames);	
}

void	Channel::kick(const std::string &target) {
	_users.erase(target);
	_operators.erase(target);
}

void Channel::leave(const std::string &user, const std::string &msg) {
	sendAllUser(msg, NULL);
	_users.erase(user);
	_operators.erase(user);
}

void	Channel::addUser(User *user) {
	_users.insert(std::make_pair(user->getNickname(), user));
}

void	Channel::setTopic(std::string &topic) {_topic = topic;}

std::string	Channel::getTopic() const {return _topic;}

void	Channel::setByInvitation(bool byInvitation, bool *changed) {
	if (byInvitation != _byInvitation)
		*changed = true;
	_byInvitation = byInvitation;
}

bool	Channel::isByInvitation() const {return _byInvitation;}

void	Channel::setTopicRestriction(bool topicRestriction, bool *changed) {
	if (topicRestriction != _topicRestriction)
		*changed = true;
	_topicRestriction = topicRestriction;
}

bool	Channel::isTopicDefRestricted() const {return _topicRestriction;}

std::string	Channel::getMode() const {
	std::string mode = "+";

	if (_byInvitation)
		mode += "i";
	if (_topicRestriction)
		mode += "t";
	if (!_password.empty())
		mode += "k";
	if (_userLimit != -1)
		mode += "l";
	return mode;
}

bool	Channel::isFull() {
	if (_userLimit != -1 && (long long)_users.size() >= _userLimit)
		return true;
	return false;
}

bool	Channel::isUser(std::string &user) {
	if (_users.find(user) != _users.end())
		return true;
	if (_operators.find(user) != _operators.end())
		return true;
	return false;
}

bool	Channel::isOperator(std::string &user) {
	if (_operators.find(user) != _operators.end())
		return true;
	return false;
}

bool	Channel::isEmpty() {return (_users.empty() && _operators.empty() ? true : false);}

bool	Channel::isPassworded() const {return !_password.empty();}

void	Channel::setPassword(std::string &password, bool *changed) {
	if (password != _password)
		*changed = true;
	_password = password;
}

std::string	Channel::getPassword() const {return _password;}

void	Channel::handleOperatorStatus(bool opStatus, std::string &user, bool *changed) {
	if (opStatus) {
		UserMap::iterator it = _users.find(user);
		if (it != _users.end()) {
			_operators.insert(std::make_pair(it->first, it->second));
			_users.erase(it);
			*changed = true;
		}
	} else {
		UserMap::iterator it = _operators.find(user);
		if (it != _operators.end()) {
			_users.insert(std::make_pair(it->first, it->second));
			_operators.erase(it);
			*changed = true;
		}
	}
}

void	Channel::setUserLimit(bool unset, std::string &userLimit, bool *changed) {
	if (unset) {
		if (_userLimit == -1)
			*changed = true;
		_userLimit = -1;
		return ;
	}
	char *endptr;
	int limit = strtol(userLimit.c_str(), &endptr, 10);
	if (*endptr != '\0' || limit < 0) // Error: bad user limit // is silent (standard RFC)
		return ;
	if (limit != _userLimit)
		*changed = true;
	_userLimit = limit;
}
