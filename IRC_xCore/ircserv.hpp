#ifndef IRCSERV
#define IRCSERV

#include <iostream>
#include <sys/socket.h> // contains definitions of function and structures , socket(), bind(), listen(), and accept(). Defines the sockaddr structure, which is a generic structure for all socket addresses.
#include <cstring> 
#include <unistd.h>
#include <string>
#include <vector>
#include <netinet/in.h>  // Contains definitions for Internet Protocol family , Defines the sockaddr_in structure, used for IPv4 addresses. , Contains macros like INADDR_ANY and functions like htons() .
#include <poll.h>
#include <fcntl.h>


class Client
{
    private:
        int clientfd;
        std::string clientIP; 
    
    public:
        Client();
        void set_clientfd(int clientfd);
        void set_clientIP(std::string clientIP);
        int get_clientfd();
        ~Client();
};

class Server
{
    private:
        int Port;
        int Serverfd;
        static bool Signal_status;
        std::vector<Client> Clients;
        std::vector<struct pollfd> fds;
    
    public:
        Server();

        void Launching_server();
        void Server_cycle(int socket_fd);


        // Socket functions:
        int socket_creation();
        sockaddr_in socket_infos(int socket_fd);
        void socket_Binding(int socket_fd, sockaddr_in serveradd);
        void socket_non_blocking(int socket_fd);
        void socket_listening(int socket_fd);
        void socket_Accepting();
        void socket_polling(int socket_fd);
        void socket_receiving();

        // Signal functions:

        static void Signal_Handler(int signal_num);

        //Removing functions:
        void Remove_Clients();
        void Close_filedescriptors();

        ~Server();
}
;

#endif