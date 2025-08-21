NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): main.cpp
	$(CXX) $(CXXFLAGS) -o $(NAME) main.cpp Client.cpp Server.cpp Channel.cpp

clean:
	@true

fclean: clean
	rm -f $(NAME)

re: fclean all
