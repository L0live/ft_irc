#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <map>
#include <list>
#include <netinet/in.h>

class User;

typedef std::map<std::string, User *>		UserMap;

class Channel {
private:
	std::string	_name;
	std::string	_password;
	std::string	_topic;
	bool		_topicRestriction;
	UserMap		_users;
	UserMap		_operators;
	std::list<std::string>	_invited;
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
	void	invite(std::string nickname);
	bool	isInvited(std::string nickname);
	// TOPIC
	void	setTopic(std::string &topic);
	std::string	getTopic() const;
	// MODE
	void	setByInvitation(bool byInvitation, bool *changed);
	bool	isByInvitation() const;
	void	setTopicRestriction(bool topicRestriction, bool *changed);
	bool	isTopicDefRestricted() const;
	std::string	getMode() const;
	bool	isFull();
	bool	isUser(std::string &user);
	bool	isOperator(std::string &user);
	bool	isEmpty();
	bool	isPassworded() const;
	void	setPassword(std::string &password, bool *changed);
	std::string	getPassword() const;
	void	handleOperatorStatus(bool opStatus, std::string &user, bool *changed);
	void	setUserLimit(bool unset, std::string &userLimit, bool *changed);
};
#endif