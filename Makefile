NAME=ircserv
CPP=c++
FLAGS=-Wall -Wextra -Werror -std=c++98
SRCS=main.cpp Server.cpp IClient.cpp User.cpp Operator.cpp Channel.cpp
OBJS=$(SRCS:.cpp=.o)
HEADERS=Server.hpp IClient.hpp User.hpp Operator.hpp Channel.hpp

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