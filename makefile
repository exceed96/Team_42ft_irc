NAME = ircserv

CPP = c++
FLAGS = -std=c++98 -Wall -Wextra -Werror -O3 -MMD -MP

COMMANDS_DIR = src/cmd
COMMANDS = $(addprefix $(COMMANDS_DIR)/, $(addsuffix .cpp, AWAY BOT DOWN INVITE JOIN KICK LIST MODE NAMES NICK NOTICE OPER PART PASS PONG PRIVMSG QUIT RESTART TOPIC WALLOPS WHO WHOIS USER))
SOURCES = main.cpp $(COMMANDS) src/Channel.cpp src/Client.cpp src/Command.cpp src/Message.cpp src/Server.cpp src/tools.cpp
HEADERS = Header/Header.hpp Header/Reply.hpp

OBJECTS = $(patsubst %.cpp,obj/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)

obj/%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -c $< -o $@

$(NAME): $(OBJECTS)
	@$(CPP) $(FLAGS) $(OBJECTS) -o $(NAME)

all: $(NAME)

clean:
	@rm -rf obj

fclean: clean
	@rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
