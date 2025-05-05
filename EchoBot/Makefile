NAME=EchoBot
CPP=c++
FLAGS=-Wall -Wextra -Werror -std=c++98 -g
SRCS=main.cpp Bot.cpp
OBJS=$(SRCS:.cpp=.o)
HEADERS=Bot.hpp

all: $(NAME)

$(NAME): $(OBJS)
	@$(CPP) $(FLAGS) $(OBJS) -o $@
	@echo "\n\nEchoBot is ready"

%.o: %.cpp
	@$(CPP) $(FLAGS) -c $<
	@echo -n '.....'

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all fclean clean re