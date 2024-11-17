#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

#include "Client.hpp"

// Canonical Form:

Client::~Client()
{}
Client::Client()
{
    this->nickname = "";
    this->clientIP = "";
    this->clientfd = -1;
    this->Operator_status = false;
    this->username = "";
    this->loged_status = 0;
}

Client::Client(Client const &cli)
{
    *this = cli;
}
Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->nickname = src.nickname;
        this->username = src.username;
        this->clientfd = src.clientfd;
        this->clientIP = src.clientIP;
        this->Operator_status = src.Operator_status;
        this->loged_status = src.loged_status;
    }
    return *this;
}

// Client getters:

std::string Client::getnickname()
{
    return this->nickname;  
}
std::string Client::getusername()
{
    return this->username;
}
std::string Client::getIPaddress()
{
    return this->clientIP;
}
int Client::get_clientfd()
{
    return this->clientfd;
}

bool Client::getregistred()
{
    return this->registration_status;
}

bool Client::getoperatorstatus()
{
    return this->Operator_status;
}

bool Client::getlogedstatus()
{
    return this->loged_status;
}

// Client setters:

void Client::setfd(int fd)
{
    this->clientfd = fd;
}
void Client::setnickname( std::string & nickName)
{
    this->nickname = nickName;
}
void Client::setusername(std::string & userName)
{
    this->username = userName;
}
void Client::setIPaddress(std::string Ipadd)
{
    this->clientIP = Ipadd;
}

void Client::setregistred(bool reg)
{
    this->registration_status = reg;
}


void Client::setlogedstatus(bool log)
{
    this->loged_status = log;
}


// int main ()
// {
//     // creating socket 
//     int client_fd = socket (AF_INET , SOCK_STREAM , 0);

//     // specfying the address
//     sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(8080);
//     server_addr.sin_addr.s_addr = INADDR_ANY;

//     // Sending connection request
//     if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
//         std::cerr << "Connection failed" << std::endl;
//         close(client_fd);
//         return 1;
//     }

//     while (true) 
//     {
//         // Sending data
//         std::string msg;
//         std::cout << "Enter message to send (or 'exit' to quit): ";
//         std::getline(std::cin, msg);
        
//         if (msg == "exit")
//             break;

//         int s = send(client_fd, msg.c_str(), msg.length(), 0);
//         if (s < 0) {
//             std::cerr<< "Send failed" << std::endl;
//             close(client_fd);
//             return 1;
//         } 
//         else
//             std::cout << s << " bytes were sent." << std::endl;
//     }
//     close(client_fd);

// }
