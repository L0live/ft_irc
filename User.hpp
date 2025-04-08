#ifndef USER_HPP
# define USER_HPP

#include "IClient.hpp"
#include <iostream>

class User : public IClient {
private:
    /* data */
public:
    User(/* args */);
    ~User();
};

#endif