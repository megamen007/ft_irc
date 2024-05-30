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
#include <arpa/inet.h>
#include <algorithm>

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
        static bool Operator_status;
        static bool Signal_status;
        std::vector<Client> Clients;
        std::vector<struct pollfd> fds;
        std::vector <std::string> msg;

    
    public:
        Server();

        void Launching_server();
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

        static void Signal_Handler(int signal_num);

        //Removing functions:
        void Remove_Client(int client_fd);
        void remove_c_from_pollfd(int id);
        void remove_c_from_Vector(int id);
        void Close_filedescriptors();
        void close_all_clients();
        void close_server_socket();

        //Parsing received DATA
        void Parcing_data_core(char *buffer);
        std::string trimming_raw_data(std::string buffer);
        void checking_trimmed_data_errors(std::string trimmed_data);
        std::string extaract_cmd(std::string trimmed_data);
        std::string   extract_arg(std::string trimmed_data);
        void Commands_errors(std::string trimmed_cmd);
        void Arguments_errors(std::string trimmed_cmd, std::string trimmed_arg);
        void one_arg_errors(std::string trimmed_cmd, std::string arg);
        void multiple_args_errors(std::string trimmed_cmd, std::string arg);
        int arguments_counter(std::string arg) ;
        void missing_arg_error(std::string trimmed_cmd);
        void creating_msg_container(std::string trimmed_cmd);
        std::vector <std::string> filling_msg_container(std::string trimmed_cmd);
        // void roles_check();
        void executing_commands();


        // Commands to Executes :
        // kick_func();  
        // invite_func();
        // mode_func(); 
        // topic_func();
        // join_func();
        // privemsg_func();



        ~Server();
}
;


#endif