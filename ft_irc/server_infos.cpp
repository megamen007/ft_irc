#include "server.hpp"

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

int Server::getPort()
{
    return this->Port;
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
        if (this->Clients[i].get_clientfd() == fd)
            return &this->Clients[i];
        i++;
    }
    return NULL;
}
Client *Server::getClientnick(std::string nickname)
{
    size_t i = 0;
    while (i < this->Clients.size())
    {
        if (this->Clients[i].getnickname() == nickname)
            return &this->Clients[i];
        i++;
    }
    return NULL;
}

Channel *Server::getChannel(std::string name)
{
    size_t i = 0;
    while (i < this->Channels.size())
    {
        if (this->Channels[i].GetName() == name)
            return &Channels[i];
        i++;
    }
    return NULL;
}

// Adding functions:

// void Server::AddChannel(Channel newChannel)
// {
//     this->channels.push_back(newChannel);
// }
// void Server::AddClient(Client newClient)
// {
//     this->Clients.push_back(newClient);
// }

// Checks Functions

bool Server::Port_valid(std::string port)
{
    std::cout << port << std::endl;
    if (port.find_first_not_of("0123456789") != std::string::npos)
    {
        std::cerr << "Invalid character found in port." <<std::endl;
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
        if (Clients[i].get_clientfd() == id)
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
        std::cout << " Client " << Clients[i].get_clientfd() << " Disconnected ..." << std::endl;
        close(Clients[i].get_clientfd());
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

void Server::registerClient(int fd, const std::vector<std::string>& messages)
 {
        Client* client = findClientByFd(fd);
        if (client == NULL)
        {
            Client newClient;
            newClient.setfd(fd);
            Clients.push_back(newClient);
            client = &Clients.back();
        }

        for (std::vector<std::string>::const_iterator it = messages.begin(); it != messages.end(); ++it)
        {
            processMessage(*client, *it);
        }
    }

    void Server::processMessage(Client& client, const std::string& message)
     {
        std::istringstream ss(message);
        std::string command;
        ss >> command;

        if (command == "PASS")
        {
            Buffer::received_pass = true;
            std::string password;
            ss >> password;
            if (password == Password)
            {
                client.setregistred(true);
                std::cout << "Client " << client.get_clientfd() << " provided valid password.\n";
            }
            else
            {
                sendError(client.get_clientfd(), "ERR_PASSWDMISMATCH");
            }
        } 
        else if (command == "NICK")
        {
            Buffer::received_nick = true;
            std::string nickname;
            ss >> nickname;
            if (isNicknameInUse(nickname))
            {
                sendError(client.get_clientfd(), "ERR_NICKNAMEINUSE");
            }
            else {
                client.setnickname(nickname);
                std::cout << "Client " << client.get_clientfd() << " set nickname: " << nickname << "\n";
            }
        } 
        else if (command == "USER")
        {
            Buffer::received_user = true;
            std::string username, hostname, servername, realname;
            ss >> username >> hostname >> servername;
            getline(ss, realname);
            if (realname.empty() || realname[0] != ':')
            {
                sendError(client.get_clientfd(), "ERR_NEEDMOREPARAMS");
            } 
            else 
            {
                client.setusername(username);
                client.setIPaddress(hostname); // Assuming this for hostname
                client.setlogedstatus(true);
                std::cout << "Client " << client.get_clientfd() << " set user info.\n";
            }
        }

        if (client.getregistred() && client.getnickname().size() > 0 && client.getusername().size() > 0)
            sendWelcome(client.get_clientfd());
    }

    Client* Server::findClientByFd(int fd)
    {
        for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it)
        {
            if (it->get_clientfd() == fd)
                return &(*it);
        }
        return NULL; // Client not found
    }

    bool Server::isNicknameInUse(const std::string& nickname)
    {
        for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it)
        {
            if (it->getnickname() == nickname)
                return true;
        }
        return false;
    }

    void Server::sendWelcome(int fd)
    {
        std::cout << "Client " << fd << " Welcome to the HAKUNA MATATA Realm.\n";
    }

    void Server::sendError(int fd, const std::string& error)
    {
        std::cerr << "Error for client " << fd << ": " << error << "\n";
        // Here you would send a real error message over the network
    }