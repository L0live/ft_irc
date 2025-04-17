#include	"Channel.hpp"
#include	<iostream>
#include	<sstream>

Channel::Channel()
	: _name(""), _userLimit(-1), _byInvitation(false){}

Channel::Channel(std::string &name)
	: _name(name), _userLimit(-1), _byInvitation(false){}

Channel::Channel(const Channel &src)
	: _name(src._name), _password(src._password), _topic(src._topic),_users(src._users), _operators(src._operators),_userLimit(src._userLimit), _byInvitation(src._byInvitation){}

Channel &Channel::operator=(const Channel &src)
{
	if (this != &src)
	{
		_password = src._password;
		_topic = src._topic; 
		_users = src._users;
		_operators = src._operators;
		_userLimit = src._userLimit; 
		_byInvitation = src._byInvitation;
	}  
	return (*this);
}

Channel::~Channel(){}

void	Channel::sendAllUser(std::string &user, std::string &msg)
{
	std::map<std::string, User *>::iterator it;
	for (it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->first != user) 
			it->second->sendChannelMsg(msg);  
	}
}

void	Channel::kickUser(std::string &user)
{
	/*
	if droit operator ok ?
	
	alors 
	
	*/
	
	_operators.erase(user);
	_users.erase(user);
}

void	Channel::addUser(User *user)
{
	/*
	if droit operator ok ? je comprend pas trop on en parle demain
	*/

	if (_userLimit > 0 && (long long)_users.size() >= _userLimit)
	{
		std::cerr << "User limit reached" << std::endl;
		return;
	}
	_users[user->getUsername()] = user;
}

void	Channel::setTopic(std::string &topic)
{
	_topic = topic;
}

std::string	Channel::getTopic() const
{
	return (_topic);
}

void	Channel::setByInvitation(bool byInvitation)
{
	_byInvitation = byInvitation;
}

bool Channel::getByInvitation() const
{
	return (_byInvitation);
}

void	Channel::setPassword(std::string &password)
{
	_password = password;
}

void	Channel::removePassword()
{
	_password.clear();
}

void	Channel::giveOperatorStatus(std::string &user)
{
	std::map<std::string, User *>::iterator it = _users.find(user);
	if (it != _users.end())
		std::cout << "operator new?" << std::endl;
		//copy ou egale operator ?!

}

void	Channel::removeOperatorStatus(std::string &user)
{
	std::map<std::string, Operator *>::iterator it = _operators.find(user);
	if (it != _operators.end())
	{
		//delete it->second si new operator ? 
		_operators.erase(it);
	}
}
