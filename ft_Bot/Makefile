NAME=ft_Bot
CPP=c++
FLAGS=-Wall -Wextra -Werror -std=c++98 -g
SRCS=main.cpp Bot.cpp
OBJS=$(SRCS:.cpp=.o)
HEADERS=Bot.hpp

all: $(NAME)

$(NAME): $(OBJS)
	@$(CPP) $(FLAGS) $(OBJS) -o $@
	@echo "\n\nft_Bot is ready"

%.o: %.cpp
	@$(CPP) $(FLAGS) -c $<
	@echo -n '.....'

clean:
	@rm -f $(OBJS)
	@echo "All objects clean"

fclean: clean
	@rm -f $(NAME)
	@echo "And the executable too"

re: fclean all

.PHONY: all fclean clean re