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

void Operator::kick(std::istringstream &request) // We must have greatest error handling
{
	std::string channel;
	std::string token;
	if (request >> channel) {
		request >> token;
		_channels.find(channel)->second->kickUser(token);
	}
}
