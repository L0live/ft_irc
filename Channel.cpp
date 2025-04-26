#include "Channel.hpp"
#include "ft_irc.hpp"

Channel::Channel() {}

Channel::Channel(std::string &name, User *user)
: _name(name), _mode("+itkol"), _userLimit(-1), _byInvitation(false) {
	_operators.insert(std::make_pair(user->getNickname(), user));
}

Channel::Channel(const Channel &src) {*this = src;}

Channel &Channel::operator=(const Channel &src) {
	if (this == &src)
		return *this;
	_name = src._name;
	_password = src._password;
	_topic = src._topic; 
	_users = src._users;
	_operators = src._operators;
	_userLimit = src._userLimit; 
	_byInvitation = src._byInvitation;  
	return *this;
}

Channel::~Channel() {}

void	Channel::sendAllUser(const std::string &msg) {
	for (UserMap::iterator it = _users.begin(); it != _users.end(); ++it) {
		send(it->second->getSockfd(), msg.c_str(), msg.size(), 0);
	}
	for (UserMap::iterator it = _operators.begin(); it != _operators.end(); it++) {
		send(it->second->getSockfd(), msg.c_str(), msg.size(), 0);
	}
}

void	Channel::kick(const std::string &target) {
	_users.erase(target);
	_operators.erase(target);
}

void Channel::leave(const std::string &user, const std::string &msg) {
	_users.erase(user);
	_operators.erase(user);
	sendAllUser(msg);
}

void	Channel::addUser(User *user) {
	_users.insert(std::make_pair(user->getNickname(), user));
}

void	Channel::setTopic(std::string &topic) {_topic = topic;}

std::string	Channel::getTopic() const {return _topic;}

void	Channel::setByInvitation(bool byInvitation) {_byInvitation = byInvitation;}

bool	Channel::getByInvitation() const {return _byInvitation;}

std::string	Channel::getMode() const {return _mode;}

void	Channel::setMode(std::string &mode) {_mode = mode;}

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

void	Channel::setPassword(std::string &password) {_password = password;}

void	Channel::removePassword() {_password = "";}

void	Channel::giveOperatorStatus(std::string &user) {
	User	*setOperator = _users.find(user)->second;
	if (!setOperator)
		return ;
	_users.erase(user);
	_operators.insert(std::make_pair(user, setOperator));
}

void	Channel::removeOperatorStatus(std::string &user){
	User *rmOperator = _operators.find(user)->second;
	if (!_operators.find(user)->second)
		return ;
	_users.insert(std::make_pair(user, rmOperator));
	_operators.erase(user);
}

void	Channel::setUserLimit(long long &userLimit) {_userLimit = userLimit;}

void	Channel::removeUserLimit() {_userLimit = -1;}
