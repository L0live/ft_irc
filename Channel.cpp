#include "Channel.hpp"
#include "ft_irc.hpp"

Channel::Channel() : _name(""), _userLimit(-1), _byInvitation(false) {}

Channel::Channel(std::string &name) : _name(name), _userLimit(-1), _byInvitation(false) {}

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

void	Channel::sendAllUser(std::string &user, std::string &msg) {
	std::string		tmpMsg = user + ": " + msg;
	for (UserMap::iterator it = _users.begin(); it != _users.end(); ++it) {
		send(it->second->getSockfd(), tmpMsg.c_str(), tmpMsg.size(), 0);
	}
}

void	Channel::kickUser(std::string &user) {
	_users.erase(user);
	_operators.erase(user);
}

void	Channel::addUser(User *user) {
	if (_userLimit > 0 && (long long)_users.size() >= _userLimit) {
		std::cerr << "User limit reached" << std::endl;
		return;
	}
	_users.insert(std::make_pair(user->getNickname(), user));
}

void	Channel::setTopic(std::string &topic) {_topic = topic;}

std::string	Channel::getTopic() const {return (_topic);}

void	Channel::setByInvitation(bool byInvitation) {_byInvitation = byInvitation;}

bool	Channel::getByInvitation() const {return _byInvitation;}

void	Channel::setPassword(std::string &password) {_password = password;}

void	Channel::removePassword() {_password = "";}

void	Channel::giveOperatorStatus(std::string &user) {
	User	*tmpUser = _users.find(user)->second;
	if (!tmpUser)
		return ;
	_users.erase(user);
	_operators.insert(std::make_pair(user, (Operator *)tmpUser));
}

void	Channel::removeOperatorStatus(std::string &user) {
	Operator	*tmpOperator = _operators.find(user)->second;
	if (!tmpOperator)
		return ;
	_operators.erase(user);
	_users.insert(std::make_pair(user, (User *)tmpOperator));
}

void	Channel::setUserLimit(long long &userLimit) {_userLimit = userLimit;}

void	Channel::removeUserLimit() {_userLimit = -1;}
