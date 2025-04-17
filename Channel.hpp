#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Operator.hpp"

class Channel {
private:
	const std::string _name;
	std::string _password;
	std::string _topic;
	//type  _restrictions; ???
	std::map<std::string, User *> _users;
	std::map<std::string, Operator *> _operators;
	long long   _userLimit;
	bool        _byInvitation;

	Channel();
public:
	Channel(std::string &name);
	Channel(const Channel &src);
	Channel	&operator=(const Channel &src);
	~Channel();

	void	sendAllUser(std::string &user, std::string &msg);
	// Operator
	void    kickUser(std::string &user);
	void	addUser(User *user);
	// TOPIC
	void	setTopic(std::string &topic);
	std::string	getTopic() const;
	// MODE
	void	setByInvitation(bool byInvitation);
	bool	getByInvitation() const;
	// void	setRestrictions(type &restrictions); ???
	// void	removeRestrictions(); ???
	void	setPassword(std::string &password);
	void	removePassword();
	void	giveOperatorStatus(std::string &user);
	void	removeOperatorStatus(std::string &user);
	void	setUserLimit(std::string &userLimit);
	// void	setUserLimit(long long &userLimit);
	void	removeUserLimit();
};



#endif