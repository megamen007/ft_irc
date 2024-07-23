CC = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

CLIENT_SRC = Client.cpp
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
NAME_CLIENT = client

SERVER_SRC = server.cpp 
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
NAME_SERVER = server

all : $(NAME_CLIENT) $(NAME_SERVER)

$(NAME_CLIENT): $(CLIENT_OBJ)
	$(CC) $(CPPFLAGS) $(CLIENT_OBJ) -o $(NAME_CLIENT)

$(NAME_SERVER): $(SERVER_OBJ)
	$(CC) $(CPPFLAGS) $(SERVER_OBJ) -o $(NAME_SERVER)

clean:
		rm -f $(CLIENT_OBJ) $(SERVER_OBJ) 

fclean : clean
		rm -f $(NAME_CLIENT) $(NAME_SERVER) 

re: fclean all

.PHONY: re fclean clean 