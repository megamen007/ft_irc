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
#include "server.hpp"
#include "Parcing_data.hpp"


class Channel;
class Server;

class Client
{
    private:
        std::string  nickname;
        std::string  username;
        std::string  hostname;
        std::string  servername;
        std::string  realname;
        std::vector<std::string> Ch_names;
        std::vector<std::string> passwords;

        int clientfd;
        std::string clientIP; 
        // channels (normal ones);
        // channels (invited only);
        bool Operator_status;
        bool registration_status;
        bool loged_status;

    public:
        // Canonical form:
            Client();
            Client(const Client& cli);
            Client &operator=(Client const &src);
            ~Client();   
        
        // Setters
            void setfd(int fd);
            void setnickname( std::string & nickName);
            void setusername(std::string & userName);
            void sethostname(std::string & hostname);
            void setservername(std::string & servername);
            void setrealname(std::string & relaname);
            void setIPaddress(std::string Ipadd);
            void setregistred(bool reg);
            void setoperatorstatus(bool oper);
            void setlogedstatus(bool log);
        
        // Getters
            std::string getnickname();
            std::string getusername();
            std::string gethostname();
            std::string getservername();
            std::string getrealname();
            std::string getIPaddress();
            std::string getPrefix();
            bool getregistred();
            bool getoperatorstatus();
            bool getlogedstatus();
            int get_clientfd();

            void executing_commands(int fd, std::string Cmd, Buffer &Parser, Client &client, Server &Excalibur);
            int JOIN(Client& client, const std::string& command, Buffer &Parser, Server &Excalibur);
            void parsing_JOIN_cmd(const std::string &cmd, std::vector<std::string>& Channel_names, std::vector<std::string>& passwords);
            void JOIN_channels(Client &client, std::vector<std::string> &Channles_names, std::vector<std::string> &passwords,  std::vector<Channel> &channels);
            bool JOIN_existing_Channel(Client &client, const std::string& channel_name, const std::string &password,  std::vector<Channel> &channels);
            void creating_new_Channel(Client &client, const std::string& channel_name, std::vector<Channel> &channels);
            void sendError(Client& client, const std::string& errorCode, const std::string& channel, const std::string& message);
            void notifyChannelJoin(Channel& channel, Client& client);


};
