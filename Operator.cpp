#include "Operator.hpp"
#include "Channel.hpp"
#include "ft_irc.hpp"

Operator::~Operator(){}


void Operator::leaveChannel(std::istringstream &request)
{
	std::string	titleChannel;
	request >> titleChannel;

	ChannelMap::iterator it = this->_channels.find(titleChannel);
	if (it != _channels.end())
		it->second->kickUser(this->getNickname());
}

void Operator::kick(std::string &target)
{
	for (ChannelMap::iterator it = this->_channels.begin(); it != this->_channels.end(); it++){
		if (it->second->getUsers().find(target) != it->second->getUsers().end())
		{
			it->second->kickUser(target);
			break;
		}
	}
}
