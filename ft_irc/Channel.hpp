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
#include "Client.hpp"

class Client;

class Channel 
{
    private:
        std::string name;
        bool invite_status;
        int limit;
        int key;
        std::string topic;
        std::string password;
        std::string creation_time; 
        std::vector <Client> Clients;
        std::vector <Client> Operators;

    public:

        // Canonical form:
            Channel();
            ~Channel();
            Channel(const Channel& src);
            Channel &operator=(const Channel &src);

        // Setters:
            void SetInvitOnly(int invit_only);
            void SetTopic(int topic);
            void SetTime(std::string time);
            void SetKey(int key);
            void SetLimit(int limit);
            void SetPassword(std::string password);
            void SetName(std::string name);

        // Getters
            int GetInvitOnly();
            int GetKey();
            int GetLimit();
            std::string GetTopic();
            std::string GetPassword();
            std::string GetName();
            std::string GetTime();

        // Client management by the channel functions:
            void add_Client(Client newClient);
            void add_Operator(Client newOperator);
};
