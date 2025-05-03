#include "Bot.hpp"

int main(int ac, char **av) {
    if (ac != 2 && ac != 3) {
        std::cerr << "Usage: " << av[0] << " <server_port> (optional)<server_password>" << std::endl;
        return 1;
    }
    try {
        Bot bot(atoi(av[1]), (ac == 3) ? av[2] : "");
        bot.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}