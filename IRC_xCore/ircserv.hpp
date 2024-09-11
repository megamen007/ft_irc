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
        std::string  nickname;
        std::string  username;
        int clientfd;
        std::string clientIP; 
        // channels (normal ones);
        // channels (invited only);
        bool Operator_status;

    public:
        // Client::Client();
        Client &Client::operator=(Client const &src);

        void Client::setfd(int fd);
        void Client::setnickname( std::string & nickName);
        void Client::setusername(std::string & userName);
        void Client::setIPaddress(std::string Ipadd);
        std::string Client::getnickname();
        std::string Client::getusername();
        std::string Client::getIPaddress();
        int Client::get_clientfd();


        Client::~Client(){};
};

class Channel {
    private:
        std::string name;
        bool invite_status;
        int limit;
        int key;
        std::string topic;
        std::string password;
        std::string creation_time; 
        std::vector <Client&> Clients;
        std::vector <Client&> Operators;
    public:
        Channel::Channel();
        Channel::~Channel();
        Channel(const Channel& src);
        Channel &Channel::operator=(const Channel &src);

        // setters & getters
        void Channel::SetInvitOnly(int invit_only);
        void Channel::SetTopic(int topic);
        void Channel::SetTime(std::string time);
        void Channel::SetKey(int key);
        void Channel::SetLimit(int limit);
        void Channel::SetPassword(std::string password);
        void Channel::SetName(std::string name);

        int Channel::GetInvitOnly();
        int Channel::GetKey();
        int Channel::GetLimit();
        std::string Channel::GetTopic();
        std::string Channel::GetPassword();
        std::string Channel::GetName();
        std::string Channel::GetTime();


        void Channel::add_Client(Client newClient);
        void Channel::add_Operator(Client newOperator);


};


class Server
{
    private:
        int Port;
        std::string Pass;
        int Serverfd;
        bool Signal_status;
        int flags_status;
        std::vector<Client> Clients;
        std::vector<struct pollfd> fds;
        std::vector <std::string> msg;

    
    public:

        Server::Server(){};

        Server &Server::operator=(Server const &src);

        // Getters

        // Setters
        void Server::setFd(int fd);
        void Server::setPort(int port);
        void Server::setPassword(std::string password);
        void Server::AddClient(Client newClient);
        
        // void Server::AddChannel(Channel newChannel);
        


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
        static void Signal_Handler(int signal_num);

        //Removing functions:
        void Remove_Client(int client_fd);
        void remove_c_from_pollfd(int id);
        void remove_c_from_Vector(int id);
        void Close_filedescriptors();
        void close_all_clients();
        void close_server_socket();

        // Parsing received DATA
        void Parcing_data_core(int client_fd, std::string buffer);
        std::string trimming_raw_data(std::string buffer);
        void checking_trimmed_data_errors(std::string trimmed_data);
        std::string extaract_cmd(std::string trimmed_data);
        std::string   extract_arg(std::string trimmed_data);
        std::string   extract_arg_v2(std::string trimmed_data);
        std::string extract_flags(std::string trimmed_data);
        void Commands_errors(std::string& cmd);
        void Arguments_errors(std::string trimmed_cmd, std::string trimmed_arg, std::string flags);
        void flags_errors(std::string cmd ,std::string flags);
        void one_flag_errors(std::string trimmed_cmd);
        void multiple_args_errors(std::string trimmed_cmd, std::string arg);
        int arguments_counter(std::string arg) ;
        void missing_flag_error(std::string trimmed_cmd, std::string arg);
        // void creating_msg_container(std::string trimmed_cmd);
        // std::vector <std::string> filling_msg_container(std::string trimmed_cmd);
        // void roles_check();
        void executing_commands(int fd, std::string Cmd);

        // Connection Registrations Commands :
        void Server::Pass_func(int fd, std::string cmd);
        // Nick_func();
        // User-func();


        // Commands to Executes :
        // kick_func();  
        // invite_func();
        // mode_func(); 
        // topic_func();
        // join_func();
        // privemsg_func();



        // Handling_User_Authentification
        bool Port_valid(std::string port); 
        Server::~Server();
}
;

#endif