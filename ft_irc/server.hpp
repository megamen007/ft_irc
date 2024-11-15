#pragma once

#include <iostream>
#include <sys/socket.h> // contains definitions of function and structures , socket(), bind(), listen(), and accept(). Defines the sockaddr structure, which is a generic structure for all socket addresses.
#include <cstring> 
#include <unistd.h>
#include <string>
#include <vector>
#include <netinet/in.h>  // Contains definitions for Internet Protocol family , Defines the sockaddr_in structure, used for IPv4 addresses. , Contains macros like INADDR_ANY and functions like htons() .
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <algorithm>

#include "Channel.hpp"
#include "Client.hpp"

class Client;
class Channel;

class Server
{
    private:
        int Port;
        std::string Password;
        static bool Signal_status;
        int Serverfd;
        int flags_status;
        std::vector<Client> Clients;
        std::vector<Channel> Channels;
        std::vector<struct pollfd> fds;
        std::vector <std::string> msg;

    public:
        Server();
        Server(const Server& srv);
        Server &operator=(Server const &src);
        ~Server();

        // Getters
        int getPort();
        int getFd();
        Client *getClient(int fd);
        Client *getClientnick(std::string nickname);
        Channel *getChannel(std::string name);
        static bool get_Signal_Status();

        // Setters
        void setFd(int fd);
        void setPort(int port);
        void setPassword(std::string password);


        void AddClient(Client newClient);
        

        // Server booting functions: 
        void Launching_server(int port, std::string password);
        void Server_cycle();


        // Socket functions:
        void socket_creation();
        sockaddr_in socket_infos();
        void socket_Binding(sockaddr_in serveradd);
        void socket_non_blocking();
        void socket_listening();
        void socket_Accepting();
        void server_socket_polling();
        void client_socket_polling(int client_fd);
        void socket_receiving(int client_fd);

        // Signal functions:
        static void Signal_Handler(int signum);

        //Removing functions:
        void Remove_Client(int client_fd);
        void remove_c_from_pollfd(int id);
        void remove_c_from_Vector(int id);
        void Close_filedescriptors();
        void close_all_clients();
        void close_server_socket();

        // Parsing received DATA
        void Parcing_and_Executing(int client_fd, std::string buffer);
        // void roles_check();
        // void executing_commands(int fd, std::string Cmd);

        // Connection Registrations Commands :
            //user_func();
            //pass_func();
            // nick_func();

        // Commands to Executes :
            // kick_func();  
            // invite_func();
            // mode_func(); 
            // topic_func();
            // join_func();
            // privemsg_func();



        // Handling_User_Authentification
        bool Port_valid(std::string port); 
        
        // void Server::AddChannel(Channel newChannel);
};



