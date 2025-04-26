#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <map>
#include <netinet/in.h>

class User;

typedef std::map<std::string, User *>		UserMap;

class Channel {
private:
	std::string _name;
	std::string _password;
	std::string _topic;
	//type  _restrictions; ???
	UserMap		_users;
	UserMap		_operators;
	long long   _userLimit;
	bool        _byInvitation;

	Channel();
public:
	Channel(std::string &name, User *user);
	Channel(const Channel &src);
	Channel	&operator=(const Channel &src);
	~Channel();

	void	sendAllUser(const std::string &msg);
	// Operator
	void    kick(const std::string &target, const std::string &msg);
	void    leave(const std::string &user, const std::string &msg);
	void	addUser(User *user);
	// TOPIC
	void	setTopic(std::string &topic);
	std::string	getTopic() const;
	// MODE
	void	setByInvitation(bool byInvitation);
	bool	getByInvitation() const;
	// void	setRestrictions(type &restrictions); ???
	// void	removeRestrictions(); ???
	bool	isUser(std::string &user);
	bool	isOperator(std::string &user);
	bool	isEmpty();
	void	setPassword(std::string &password);
	void	removePassword();
	void	giveOperatorStatus(std::string &user);
	void	removeOperatorStatus(std::string &user);
	void	setUserLimit(long long &userLimit);
	void	removeUserLimit();
};



#endif