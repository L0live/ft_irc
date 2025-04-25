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
	Channel(std::string &name);
	Channel(const Channel &src);
	Channel	&operator=(const Channel &src);
	~Channel();

	void	sendAllUser(const std::string &user, std::string msg);
	// Operator
	void    kickUser(const std::string &user);
	void    Leave(const std::string &user);
	void	addUser(User *user);
	// TOPIC
	void	setTopic(std::string &topic);
	std::string	getTopic() const;
	// MODE
	void	setByInvitation(bool byInvitation);
	bool	getByInvitation() const;
	// void	setRestrictions(type &restrictions); ???
	// void	removeRestrictions(); ???
	UserMap	getUsers();
	void	setPassword(std::string &password);
	void	removePassword();
	void	giveOperatorStatus(std::string &user);
	void	removeOperatorStatus(std::string &user);
	void	setUserLimit(long long &userLimit);
	void	removeUserLimit();
};



#endif