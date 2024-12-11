#include "server.hpp"
#include <stdlib.h>
// Canonical Form:

Server::Server()
{
}
Server::Server(const Server &srv)
{
    *this = srv;
}

Server::~Server()
{
}

Server &Server::operator=(Server const &src)
{
    if (this != &src)
    {
        this->Port = src.Port;
        this->Serverfd = src.Serverfd;
        this->Password = src.Password;
        this->Clients = src.Clients;
        // this->Channels = src.Channels;
        this->fds = src.fds;
    }
    return *this;
}

// Server setters :

void Server::setFd(int fd)
{
    this->Serverfd = fd;
}

void Server::setPassword(std::string password)
{
    this->Password = password;
}

void Server::setPort(int port)
{
    this->Port = port;
}

//  Server getters :

std::vector<Channel *> Server::get_Channels()
{
    return this->Channels;
}

int Server::getPort()
{
    return this->Port;
}

std::string Server::getRawData()
{
    return this->RawData;
}

int Server::getFd()
{
    return this->Serverfd;
}

int Server::get_Signal_Status()
{
    return this->Signal_status;
}

Client *Server::getClient(int fd)
{
    size_t i = 0;
    while (i < this->Clients.size())
    {
        if (this->Clients[i]->get_clientfd() == fd)
            return this->Clients[i];
        i++;
    }
    return NULL;
}

Client *Server::getClientname(std::string nickname)
{
    size_t i = 0;
    while (i < this->Clients.size())
    {
        if (this->Clients[i]->getnickname() == nickname)
            return this->Clients[i];
        i++;
    }
    return NULL;
}

Channel *Server::getChannel(std::string name)
{
    size_t i = 0;
    while (i < this->Channels.size())
    {
        if (this->Channels[i]->GetName() == name)
            return Channels[i];
        i++;
    }
    return NULL;
}

// Checks Functions

bool Server::Port_valid(std::string port)
{
    if (port.find_first_not_of("0123456789") != std::string::npos)
    {
        std::cerr << "Invalid character found in port." << std::endl;
        return false;
    }

    int portnum = std::atoi(port.c_str());

    if (portnum >= 1024 && portnum <= 65535)
        return true;
    else
        return false;
}

// Close & Remove Functions

void Server::remove_c_from_pollfd(int id)
{
    size_t i = 0;
    while (i < fds.size())
    {
        if (fds[i].fd == id)
            fds.erase(fds.begin() + i);
        i++;
    }
}
void Server::remove_c_from_Vector(int id)
{
    size_t i = 0;
    while (i < Clients.size())
    {
        if (Clients[i]->get_clientfd() == id)
            Clients.erase(Clients.begin() + i);
        i++;
    }
}
void Server::Remove_Client(int id)
{
    remove_c_from_pollfd(id);
    remove_c_from_Vector(id);
}

void Server::close_all_clients()
{
    size_t i = 0;
    while (i < Clients.size())
    {
        std::cout << " Client " << Clients[i]->get_clientfd() << " Disconnected ..." << std::endl;
        close(Clients[i]->get_clientfd());
        i++;
    }
}

void Server::close_server_socket()
{
    if (Serverfd != -1)
    {
        std::cout << " Server  Disconnected " << std::endl;
        close(Serverfd);
    }
}

void Server::Close_filedescriptors()
{
    close_all_clients();
    close_server_socket();
}

bool Server::Valid_nick_name(std::string& nickname)
{
    size_t i = 0;
    if (!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
        return false;
    while(i < nickname.size() )
    {
        if (!std::isalnum(nickname[i]) && nickname[i] != '_')
            return false;
        i++;
    }
    return true;
}


Client *Server::findClientByFd(int fd)
{
    for (std::vector<Client *>::iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if ((*it)->get_clientfd() == fd)
            return *it;
    }
    return NULL;
}

void Server::sendWelcome(int fd)
{
    std::cout << "Client " << fd << " Welcome to the HAKUNA MATATA Realm $IRC SERVER$.\n";
}


std::string Server::trim(std::string &str)
{
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
    return str;
}

