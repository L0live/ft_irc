NAME=ircserv
CPP=c++
FLAGS=-Wall -Wextra -Werror -std=c++98  -g
SRCS=main.cpp Server.cpp User.cpp Channel.cpp
OBJS=$(SRCS:.cpp=.o)
HEADERS=Server.hpp User.hpp Channel.hpp

all: $(NAME)

$(NAME): $(OBJS)
	@$(CPP) $(FLAGS) $(OBJS) -o $@
	@echo "\n\nIRC server is ready"

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