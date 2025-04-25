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

void	Channel::sendAllUser(const std::string &user, std::string msg) {
	std::string		tmpMsg = user + ": " + msg;
	for (UserMap::iterator it = _users.begin(); it != _users.end(); ++it) {
		send(it->second->getSockfd(), tmpMsg.c_str(), tmpMsg.size(), 0);
	}
	for (UserMap::iterator it = _operators.begin(); it != _operators.end(); it++) {
		//std::string command = RPL_KICK(client, channel, target, reason);
	}
}

void	Channel::kickUser(const std::string &user) {
	_users.erase(user);
	_operators.erase(user);
	sendAllUser(user, "kickkkkk");
}

void Channel::Leave(const std::string &user)
{
	_users.erase(user);
	_operators.erase(user);	
	sendAllUser(user, "leave");
#define ERR_NOTONCHANNEL(client, channel)			(": 442 " + client + " " + channel + " :You're not on that channel\r\n")
#define RPL_PART(client, channel)					(":" + client + " PART " + channel + "\r\n")
#define RPL_PARTMESSAGE(client, channel, message)	(":" + client + " PART " + channel + " :" + message +"\r\n")

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

UserMap Channel::getUsers(){
	return(this->_users);
}

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
