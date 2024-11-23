CC = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

SRC = main.cpp server.cpp server_infos.cpp Channel.cpp Client.cpp Parcing_data.cpp Channel_cmds.cpp 

OBJ = $(SRC:.cpp=.o)

NAME = ft_irc

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: re fclean clean 