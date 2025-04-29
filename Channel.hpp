#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <map>
#include <netinet/in.h>

class User;

typedef std::map<std::string, User *>		UserMap;

class Channel {
private:
	std::string	_name;
	std::string	_password;
	std::string	_topic;
	bool		_topicRestriction;
	std::string	_mode;
	UserMap		_users;
	UserMap		_operators;
	int			_userLimit;
	bool		_byInvitation;

	Channel();
public:
	Channel(std::string &name, User *user);
	Channel(const Channel &src);
	Channel	&operator=(const Channel &src);
	~Channel();

	void	sendAllUser(std::string msg, std::string *nick);
	std::string	who();
	// Operator
	void    kick(const std::string &target);
	void    leave(const std::string &user, const std::string &msg);
	void	addUser(User *user);
	// TOPIC
	void	setTopic(std::string &topic);
	std::string	getTopic() const;
	// MODE
	void	setByInvitation(bool byInvitation);
	bool	isByInvitation() const;
	void	setTopicRestriction(bool topicRestriction);
	bool	isTopicDefRestricted() const;
	std::string	getMode() const;
	void	setMode(std::string &mode);
	bool	isFull();
	bool	isUser(std::string &user);
	bool	isOperator(std::string &user);
	bool	isEmpty();
	bool	isPassworded() const;
	void	setPassword(std::string &password);
	std::string	getPassword() const;
	void	handleOperatorStatus(bool opStatus, std::string &user);
	void	setUserLimit(bool unset, std::string &userLimit);
};
#endif