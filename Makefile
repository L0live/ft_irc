EXEC = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g
OBJDIR = obj
SRCDIR = src
INCDIR = include
INC = i$(INCDIR)/Server.hpp $(INCDIR)/User.hpp $(INCDIR)/Channel.hpp
SRC = $(SRCDIR)/main.cpp $(SRCDIR)/Server.cpp $(SRCDIR)/User.cpp $(SRCDIR)/Channel.cpp
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(EXEC): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $@
	@echo "\n\nIRC server is ready"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)
	@mkdir -p $(OBJDIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo -n '.....'

bonus: $(EXEC)
	@$(MAKE) --no-print-directory -C EchoBot

all: $(EXEC)

clean:
	@rm -rf $(OBJDIR)
	@$(MAKE) --no-print-directory -C EchoBot clean
	@echo "All objects clean"

fclean: clean
	@rm -f $(EXEC)
	@$(MAKE) --no-print-directory -C EchoBot fclean
	@echo "And the executable too"

re: fclean all

.PHONY: all clean fclean re