#include "Bot.hpp"

Bot::Bot() {}

Bot::Bot(int servPort, std::string servPassword)
    : _nickname("EchoBot"), _username(_nickname),  _servPort(servPort), _servPassword(servPassword) {init();}

Bot::~Bot() {
    if (_servSockfd != -1) {
        // sendRequest("QUIT :Shutting down\r\n");
        close(_servSockfd);
    }
}

void	handleSIGINT(int) {throw std::runtime_error("\nBot shutting down...");}

void	Bot::init() {
	signal(SIGINT, handleSIGINT); // On gère le signal SIGINT (Ctrl+C)

    // Creation du socket du bot
    _servSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_servSockfd < 0) {
        throw std::runtime_error("Error: creating socket");
    }

    // On initialise des trucs
    int opt = 1;
    setsockopt(_servSockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&_servAddr, '\0', sizeof(_servAddr));
    _servAddr.sin_family = AF_INET;
    _servAddr.sin_port = htons(_servPort);
    inet_pton(AF_INET, "localhost", &_servAddr.sin_addr);
}

void Bot::run() {
    if (connect(_servSockfd, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) < 0) {
        throw std::runtime_error("Error: connecting to server");
    }
    std::cout << "Connected to server on " << inet_ntoa(_servAddr.sin_addr) << ":" << _servPort << std::endl;
    std::string registrationMsg;
    if (!_servPassword.empty())
        registrationMsg = "PASS " + _servPassword + "\r\n";
    registrationMsg += "NICK " + _nickname + "\r\n";
    registrationMsg += "USER " + _username + " 0 * :realname\r\n";
    sendRequest(registrationMsg);
    while (true) {
        std::string response = receiveRequest();
        if (response.empty())
            throw std::runtime_error("Error: server disconnected");
        std::istringstream request(response);
        interpretRequest(request);
    }
}

std::string Bot::receiveRequest() {
    char buffer[1024];

    memset(buffer, '\0', sizeof(buffer));
    recv(_servSockfd, buffer, sizeof(buffer) - 1, 0);
    std::cout << "Received: " << buffer << std::endl;
    return std::string(buffer);
}

void Bot::sendRequest(std::string msg) {
    std::cout << "Sended: " << msg << std::endl;
    send(_servSockfd, msg.c_str(), msg.size(), MSG_NOSIGNAL);
}

void Bot::interpretRequest(std::istringstream &request) {
    std::string	token;
    request >> token;
    if (token == ":")
        request >> token;
    if (token == "464")
        throw std::runtime_error("Error: Password incorrect");
    else if (token == "433") {
        _nickname += "_";
        sendRequest("NICK " + _nickname + "\r\n");
    } else if (token == "001")
        std::cout << "Bot successfully register." << std::endl;
    else if (token == "341") {
		request >> token;
		request >> token;
		request >> token;
   		sendRequest("JOIN " + token + "\r\n");
	}
	else {
        sendMsg(request, token);
    }
}

void Bot::sendMsg(std::istringstream &request, std::string &client) { // TODO: add other rules to bot (that begin with '!')
    std::string token;
    std::string msg;
    request >> token; // PRVMSG
    if (token != "PRIVMSG")
		return ;
	request >> token; // target (WE)
	if (token[0] == '#')
		client = token;
    request >> token;
    if (token[0] == ':')
        token.erase(token.begin());
    do { msg += token + " "; } while (request >> token);
	if (client[0] == '#')
		sendRequest("PRIVMSG " + client + " :" + msg + "\r\n");
	else
    	sendRequest("PRIVMSG " + client.substr(1, client.find('!') - 1) + " :" + msg + "\r\n");
}