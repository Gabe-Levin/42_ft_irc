NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

OBJDIR = obj
SRCS = main.cpp Client.cpp Server.cpp Channel.cpp \
       Commands/Invite.cpp Commands/Join.cpp Commands/Kick.cpp \
       Commands/Mode.cpp Commands/Nick.cpp Commands/Pass.cpp \
       Commands/Privmsg.cpp Commands/Topic.cpp Commands/User.cpp \
	   Commands/Part.cpp Commands/Names.cpp

OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
