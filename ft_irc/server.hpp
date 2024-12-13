#pragma once

#include <iostream>
#include <errno.h>
#include <sys/socket.h> // contains definitions of function and structures , socket(), bind(), listen(), and accept(). Defines the sockaddr structure, which is a generic structure for all socket addresses.
#include <cstring>
#include <csignal>
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

// TOPIc error messages //
#define ERR_INCORPASS(client)                           (" 464 " + client + " :Password incorrect\r\n")
#define ERR_NOTENOUGHPARAM(client)                      (" 461 " + client + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(nick)						(" 462 " + nick + " :You may not reregister\r\n")
#define RPL_WELCOME(nick)						(" 001 " + nick + " :Welcome to the Hakuna Matata IRC Network, " + nick + "\r\n")

// #define RPL_WELCOME(nick)						(" 001 :Welcome to the " + "Hakuna Matata " + "IRC Network, " + nick + "\r\n")
#define ERR_PASSWDMISMATCH()							(" 464 :Password incorrect\r\n")
#define RPL_ENDOFWHOIS(cli, chan)						(" 315 " + cli + " " + chan + " :End of /WHOIS list\r\n")
#define RPL_ENDOFNAMES(cli, chan)					(" 366 " + cli + " " + chan + " :End of /NAMES list\r\n")
#define ERR_NOSUCHNICK(client,nick)						(" 401 "  + client + " " + nick + ":No such nick/channel\r\n")
#define ERR_NOTREGISTERED(nickname)                     (": 451 " + nickname + " :You have not registered!\r\n")

// CHANNEL error messages //
#define ERR_NEEDMOREPARAMS(client, command)				(" 461 " + client + " " + command + " :Not enough parameters\r\n")
#define ERR_NEEDMOREPARAMSS()							(" 461 :Not enough parameters\r\n")
#define ERR_USERNOTINCHANNEL(client, nick, chan) 		(" 441 " + client + " " + nick + " " + chan + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(client, chan)  				(" 442 " + client + " " + chan +  " :You're not on that channel\r\n")
#define ERR_USERONCHANNEL(client, nick, chan)			(" 443 " + client + " " + nick + " " + chan +  " :is already on channel\r\n")
#define ERR_NOSUCHCHANNEL(client, chan)	   				(" 403 " + client + " " + chan + " :No such channel\r\n")
#define ERR_UNKNOWNCOMMAND(client, command)				(" 421 " + client + " " + command + " :Unknown command\r\n")

// MODE error messages //
#define ERR_UNKNOWNMODE(nick, mode)						(" 472 " + nick + " " + mode + " :is unknown mode char to me\r\n")
#define	RPL_CHANNELMODEIS(cli, chan, mode)				(" 324 " + cli + " " + chan + " " + mode + "\r\n")
#define RPL_CREATIONTIME(cli, chan, creationTime)   	(" 329 " + cli + " " + chan + " " + creationTime + "\r\n")
#define ERR_CHANOPRIVSNEEDED(client, chan)				(" 482 " + client + " " + chan +  " :You're not channel operator\r\n")
#define ERR_NICKNAMEINUSE(user, nickname)   (" 433 " + user + " " + nickname + " :Nickname is already in use\r\n")
#define ERR_NONICKNAMEGIVEN(user)           (" 431 " + user + " : No nickname given\r\n")  
#define ERR_ERRONEUSNICKNAME(user, nick)    (" 432 " + user + " " + nick + " :Erroneus nickname\r\n");

#define RPL_NOTOPIC(cli, chan)							(" 331 " + cli + " " + chan + " :No topic is set\r\n")
#define RPL_TOPIC(cli, chan, topic)						(" 332 " + cli + " " + chan + " :" + topic + "\r\n")
#define	RPL_TOPICWHOTIME(user, chan, setter, time)		(" 333 " + user + " " + chan + " "  + setter + " " + time + "\r\n")
#define ERR_INVALIDMODEPARAM(nick, chanuser, mode, param, desc)		(" 696 " + nick + " " + chanuser + " " + mode + " " + param + " " + desc + "\r\n")
#define ERR_CHANOPRIVSNEEDED(client, chan)				(" 482 " + client + " " + chan +  " :You're not channel operator\r\n")
#define RPL_INVITING(client, nick, chan)				(" 341 " + client + " " + nick + " " + chan + "\r\n")
#define ERR_USERONCHANNEL(client, nick, chan)			(" 443 " + client + " " + nick + " " + chan +  " :is already on channel\r\n")

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
        std::string RawData;
        std::vector<struct pollfd> fds;
        std::vector <std::string> msg;

    public:

        std::vector<Client *> Clients;
        std::vector<Channel *> Channels;
        std::vector<std::string> jokes;
        std::vector<std::string> insults;
        Server();
        Server(const Server& srv);
        Server &operator=(Server const &src);
        ~Server();

        // Getters
        int getPort();
        int getFd();
        int get_Signal_Status();
        bool getpsdverified();
        std::string getRawData();
        std::vector<Channel *> get_Channels();

        // Setters
        void setFd(int fd);
        void setPort(int port);
        void setPassword(std::string password);


        void addClient(Client newClient);
        void addChannel(Channel *chan);


        // Server booting functions:
        void Launching_server(int port, std::string password);
        void Server_cycle();


        // Socket functions:
        void socket_creation();
        sockaddr_in socket_infos();
        void socket_Binding(sockaddr_in serveradd);
        void socket_non_blocking();
        void socket_listening();

        // void socket_Accepting();
        void socket_Accepting();
        void server_socket_polling();
        void client_socket_polling(int client_fd);
        void socket_receiving(int client_fd);
        std::string getServerIP();

        void removeChannel(Channel *chan);

        // Signal functions:
        void cleanupServer();
        static void Signal_Handler(int signum);

        //Removing functions:
        void Remove_Client(int client_fd);
        void remove_c_from_pollfd(int id);
        void Close_filedescriptors();
        void close_all_clients();
        void close_server_socket();
        
        // Parsing received DATA
        void    Parcing_and_Executing(int fd ,std::string buffer);

        std::vector<std::string>  split_received_Buffer(std::string str);
        std::vector<std::string>  split_cmd(std::string &cmd);

        Client* findClientByFd(int fd);
        Client* findClientByNick(const std::string& nickname);
        bool isNicknameInUse(const std::string& nickname);
        void sendWelcome(int fd);
        std::string  trim(std::string &str);
        Channel * create_channel(Client *cl, std::string name, std::string pass);

        bool Valid_nick_name(std::string& nickname);
        bool Port_valid(std::string port);
        void ssendMessage(std::string message, int destination_fd);

        void checkRegistration(Client *client);
        void    executing_commands(int fd, std::string &cmd);
        Client  *getClient(int fd);
        Client  *getClientname(std::string nickname);
        Channel *getChannel(std::string name);

        void JOIN(Client* client, std::string &line);
        void MODE( Client *client, std::string &line);
        void KICK(Client *admin,std::string &line);
        void INVITE(Client *admin,std::string &line);
        void TOPIC(Client *admin,std::string &line);
        void PART(Client *admin,std::string &line);
        void NICK(Client *admin,std::string &line);
        void USER(Client *admin,std::string &line);
        void PASS(Client *admin,std::string &line);
        void PRIVMSG(Client *admin,std::string &line);
        void WHO(Client* admin, std::string &line);
        void QUIT(Client *clio, std::string &line);
        void Bot_call(Client *admin, std::string param);//
        
};



