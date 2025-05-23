#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <netdb.h>
#include <cmath>
#include <unistd.h>

class Bot {
private:
    std::string _nickname;
    std::string _username;

    // Server information
    int _servSockfd;
    int _servPort;
    sockaddr_in _servAddr;
    std::string _servPassword;

    Bot();
    void init();
    std::string receiveRequest();
    void sendRequest(std::string msg);
    void interpretRequest(std::istringstream &request);
    void sendMsg(std::istringstream &request, std::string &client);
public:
    Bot(int servPort, std::string servPassword);
    ~Bot();

    void run();
};

#endif
