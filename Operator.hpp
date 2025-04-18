#ifndef OPERATOR_HPP
# define OPERATOR_HPP

#include "ft_irc.hpp"

class Operator : public User {
public:
    ~Operator();

	void	leaveChannel();
	void	kick(std::string &target);
	// void	kick(std::string &target, Channel *channel);
	// void	kick(std::string &target, std::string &channel);
    void    invite(std::string &target); 
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
	void	removeUserLimit();
};

#endif