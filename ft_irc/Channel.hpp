#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include "Client.hpp"
#include <ctime>
#include "server.hpp"
#include <cstdlib>

// TOPIc error messages //
#define RPL_NOTOPIC(cli, chan)							(" 331 " + cli + " " + chan + " :No topic is set\r\n")
#define RPL_TOPIC(cli, chan, topic)						(" 332 " + cli + " " + chan  + " :" + topic + "\r\n")
#define RPL_WELCOME(nick)						(" 001 " + nick + " :Welcome to the Hakuna Matata IRC Network, " + nick + "\r\n")

// #define RPL_WELCOME()									(" 001 :Welcome to the Internet Relay Network \r\n")
#define ERR_PASSWDMISMATCH()							(" 464 :Password incorrect\r\n")
#define RPL_ENDOFWHOIS(cli, chan)						(" 315 " + cli + " " + chan + " :End of /WHOIS list\r\n")
// #define RPL_ENDOFNAMES(cli, chan)					(" 366 " + cli + " " + chan + " :End of /NAMES list\r\n")
#define ERR_NOSUCHNICK(client,nick)						(" 401 "  + client + " " + nick + ":No such nick/channel\r\n")

// CHANNEL error messages //
#define ERR_NEEDMOREPARAM(client, command)				(" 461 " + client + " " + command + " :Not enough parameters\r\n")
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
#define RPL_INVITING(client, nick, chan)				(" 341 " + client + " " + nick + " " + chan + "\r\n")
#define ERR_INVITEONLYCHAN(nick, chan)								(" 473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n")
#define ERR_CHANNELISFULL(nick, chan)								(" 471 " + nick + " " + chan + " :Cannot join channel (+l)\r\n")
#define ERR_BADCHANNELKEY(nick, chan)								(" 475 " + nick + " " + chan + " :Cannot join channel (+k)\r\n")
// #define ERR_BADCHANNELKEY(nick, chan)									(" 525 " + nick + " " + chan + " :Key is not well-formed\r\n")

class Client;
class Channel
{
    private:

        std::string name;
        std::string topic;
        std::string mode;
        std::string password;
        std::time_t creation_time;
        std::time_t topic_time;
        bool invite_only;
        bool has_password;
        bool has_topic;
        bool has_rest;
        bool has_limit;
        bool operate;
        size_t max_users;
        size_t users;

    public:

        std::vector<Client *> admins;
        std::vector<Client *> Clients;
        std::vector<Client *> invites;

        Channel();
        Channel(const std::string& name);
        Channel(const std::string& name, const std::string& pswd);
        ~Channel();
        Channel(const Channel &src);
        Channel &operator=(const Channel &src);
        
        void addAdmin(Client* client);
        void addUser(Client* client,  std::string pass, std::string serverIP);
        void addInvited(Client* client);
    

        void SetLimit(int limit);
        void SetMaxUsers(int max);
        void SetName(std::string name);
        void SetMode(char mode);
        void SetTopic(std::string topic);
        void SetPassword(std::string password);
        void SetTime(std::time_t time);
        void SetAdmin(Client *param);
        void sendMessage(std::string message, int destination_fd);
        void set_has_password(bool has_password);

        std::string GetName();
        std::time_t get_topictime();
        std::string get_modes();
        std::time_t get_time();
        std::string get_topic();
        std::vector<std::string> getinvites();
        std::string GetUserInfo(Client *admin, bool i);
        Client     *GetUser(std::string name);
        Client *getOperator(std::string name);
        std::vector<Client *>  getMembers();
        // Client     *Get_Operator(std::string name);
        bool get_invite_only();
        bool get_limit();
        size_t get_maxusers();
        std::string get_password();
        bool get_has_password();
        bool get_has_topic();
        // Client & get_Operator(std::string  & client_nick)
        size_t GetClientsNumber();
        bool is_Admin(Client *admin);
        bool is_Invited(Client *admin);
        bool onChannel(Client *admin); 
        bool is_inChannel(Client *admin);
        Client* GetClientInChannel(std::string name);
        void admin_MODE(Client *admin, std::string mode, std::string arg);
        void changeInviteMode(Client *admin, bool i);
        void changeKeyMode(Client *admin, std::string key, bool i, std::string serverIPadd);
        void changeTopicMode(Client *admin, bool i);
        void add_admin(Client *admin, std::string name, std::string serverIPadd);
        void remove_admin(Client *admin);
        void remove_admino(Client *admin, std::string name,std::string serverIPadd);
        void remove_Invited(Client *admin);
        void change_MaxUser(Client *admin, int i, std::string &param, std::string serverIPadd);
        void send_to_all(std::string message);
        void send_to_all_except_me(std::string message, Client *clio);
        void remove_user(Client *admin);
        bool validPassio(std::string passio);
        char *getMessage();
        void rpl_topic(Client *cli, std::string topic,std::string serverIPadd);
        void rpl_list(Client *cli, std::string serverIPadd);
        void rpl_mode(Client *cli, std::string serverIPadd);
        void valid_mode(Client *cli, std::string &modes, std::string param, std::string serverIPadd);
        void plus_modes(Client *cli, char c, std::string param, std::string serverIPadd);
        void minus_modes(Client *cli, char c, std::string param, std::string serverIPadd);
    
};