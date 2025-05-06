#include "../include/ft_irc.hpp"

int main(int argc, char **argv) {
	if (argc != 2 && argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> (optional)<password>" << std::endl;
		return 1;
	}
	try {
		Server server(argv[1], argc == 3 ? argv[2] : "");
		server.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}