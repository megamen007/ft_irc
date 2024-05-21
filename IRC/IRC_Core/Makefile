CC = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

CLIENT_SRC = Client.cpp

CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

NAME_CLIENT = client

SERVER_SRC = Server.cpp 

SERVER_OBJ = $(SERVER_SRC:.c=.o)

NAME_SERVER = server

all : $(NAME_CLIENT) $(NAME_SERVER)

$(NAME_CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(CLIENT_OBJ) -o $(NAME_CLIENT)

$(NAME_SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(SERVER_OBJ) -o $(NAME_SERVER)

clean:
		rm -f $(CLIENT_OBJ) $(SERVER_OBJ) 

fclean : clean
		rm -f $(NAME_CLIENT) $(NAME_SERVER) 

re: fclean all

.PHONY: re fclean clean 