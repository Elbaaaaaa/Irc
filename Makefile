NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -Iincludes

SRCS = srcs/main.cpp srcs/Server.cpp srcs/Client.cpp srcs/Channel.cpp srcs/Parser.cpp \
	srcs/Commands_connection.cpp srcs/Commands_messages.cpp srcs/Commands_channel.cpp srcs/Commands_mode.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(OBJS)

re: fclean all

.PHONY: all clean fclean re

.PHONY: parser_test
parser_test:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) test_parser.cpp srcs/Parser.cpp -o test_parser
	./test_parser