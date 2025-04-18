#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#include <iostream>
#include <cstring>
#include <csignal>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cmath>

class Channel;
class User;
class Operator;

typedef std::map<std::string, Channel *>	ChannelMap;
typedef std::map<std::string, User *>		UserMap;
typedef std::map<std::string, Operator *>	OperatorMap;

#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "Operator.hpp"

#endif