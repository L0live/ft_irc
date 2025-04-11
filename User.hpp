#ifndef USER_HPP
# define USER_HPP

#include "IClient.hpp"

class User : public IClient {
public:
    ~User();

	void	leaveChannel();
};

#endif