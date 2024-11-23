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

std::vector<Channel> Server::get_Channels()
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

void Server::registerClient(int fd, std::string raw)
{
    Client *client = findClientByFd(fd);
    if (client == NULL)
    {
        // std::cout << " client is null?\n";
        Client newClient;
        newClient.setfd(fd);
        Clients.push_back(newClient);
        client = &Clients.back();
    }

    std::string command;
    std::string arg;
    std::string message;

    if (raw.find("\r\n") != std::string::npos)
    {
        std::vector<std::string> lines;
        lines = splitByCRLF(raw);
        for (size_t i = 1; i < lines.size(); i++)
        {
            std::istringstream ss(lines[i]);
            ss >> command;
            ss >> arg;
            std::getline(ss, message);
            processMessage(*client, command, arg, message);
        }
    }
    else
    {
        std::istringstream ss(raw);
        ss >> command;
        ss >> arg;
        std::getline(ss, message);
        processMessage(*client, command, arg, message);
    }
}

void Server::processMessage(Client &client, const std::string &command, const std::string &arg, const std::string &msg)
{
    std::istringstream ss(arg);
    if (command == "PASS")
    {
        std::string password;
        ss >> password;
        // std::cout << password << "[][][]" <<  Password;

        if (password == Password)
        {
            client.setregistred(true);
            Buffer::received_pass = true;
            std::cout << "Client " << client.get_clientfd() << " provided valid password.\n";
        }
        else
        {
            sendError(client.get_clientfd(), "ERR_PASSWDMISMATCH");
        }
    }
    else if (command == "NICK")
    {
        std::string nickname;
        ss >> nickname;
        if (isNicknameInUse(nickname))
        {
            sendError(client.get_clientfd(), "ERR_NICKNAMEINUSE");
        }
        else
        {
            std::cout << nickname << " ngalza" << std::endl;
            // client.setnickname(nickname);
            n_name = nickname;
            Buffer::received_nick = true;
            std::cout << "Client " << client.get_clientfd() << " set nickname: " << nickname << "\n";
        }
    }
    else if (command == "USER")
    {
        std::istringstream ss(msg);
        std::string username, hostname, servername, realname, tmp;
        username = arg;
        ss >> hostname >> servername;
        getline(ss, tmp);
        realname = trim(tmp);

        std::cout << username << std::endl;
        std::cout << hostname << std::endl;
        std::cout << servername << std::endl;
        std::cout << realname << std::endl;

        if (realname.empty())
        {
            sendError(client.get_clientfd(), "ERR_NEEDMOREPARAMS");
        }
        else
        {
            u_name = username;
            h_name = hostname;
            s_name = servername;
            r_name = realname;
            // client.setusername(username);
            // client.setservername(servername);
            // client.setrealname(realname);
            // client.sethostname(hostname);
            // client.setIPaddress(hostname); // Assuming this for hostname
            // client.setlogedstatus(true);
            // client.setregistred(true);
            Buffer::received_user = true;
            std::cout << "Client " << client.get_clientfd() << " set user info.\n";
        }
    }

    if (client.getregistred() && client.getnickname().size() > 0 && client.getusername().size() > 0)
        sendWelcome(client.get_clientfd());
}

Client *Server::findClientByFd(int fd)
{
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if (it->get_clientfd() == fd)
            return &(*it);
    }
    return NULL; // Client not found
}

bool Server::isNicknameInUse(const std::string &nickname)
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

void Server::sendError(int fd, const std::string &error)
{
    std::cerr << "Error for client " << fd << ": " << error << "\n";
    // Here you would send a real error message over the network
}

std::string Server::trim(std::string &str)
{
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
    return str;
}

std::vector<std::string> Server::splitByCRLF(const std::string &input)
{
    std::vector<std::string> result;
    std::string::size_type start = 0, end;

    while ((end = input.find("\r\n", start)) != std::string::npos)
    {
        result.push_back(input.substr(start, end - start));
        start = end + 2; // Skip past "\r\n"
    }

    if (start < input.length())
        result.push_back(input.substr(start));

    return result;
}

Channel &Client::JOIN(Client& client, const std::string& command, __unused Buffer &Parser, Server &Excalibur)
{
    // std::cout << client.getnickname() << std::endl;
    std::vector<Channel> tmp;
    tmp = Excalibur.get_Channels();

    Channel new_channel;
    parsing_JOIN_cmd(command, Ch_names, passwords);
    new_channel = JOIN_channels(client , Ch_names, passwords, tmp);
    return new_channel;
}

void Client::parsing_JOIN_cmd(const std::string &cmd, std::vector<std::string>& Channel_names, std::vector<std::string>& passwords)
{
    size_t del = cmd.find(' ');
    std::string Ch_part = cmd.substr(5 , del - 5);
    std::string pass_part = cmd.substr(del + 1);

    std::istringstream Ch_stream(Ch_part);
    std::string Ch_name;
    while(std::getline(Ch_stream, Ch_name, ','))
    {
        Channel_names.push_back(Ch_name);
    }

    std::istringstream pass_stream(pass_part);
    std::string pass;
    while(std::getline(pass_stream, pass, ','))
    {
        passwords.push_back(pass);
    }

}

Channel & Client::JOIN_channels(Client &client, std::vector<std::string> &Channles_names, std::vector<std::string> &passwords, std::vector<Channel> &channels)
{
    for(size_t i = 0 ; i < Channles_names.size() ; ++i)
    {
        std::string Ch_name = Channles_names[i];
        std::string password;

        if (i < password.size())
        {
            password = passwords[i];
        }
        else    password = "";


        if (Ch_name.find("#") != 0)
        {
            sendError(client , "403", Ch_name, "NO such ");//err
            continue;
        }

        Channel new_channel;
        if(!JOIN_existing_Channel(client, Ch_name, password, channels))
            new_channel = creating_new_Channel(client, Ch_name, channels);
        else
        {
            for(std::vector<Channel>::iterator it = channels.begin() ; it != channels.end(); it++)
            {
                if(it->GetName() == Ch_name)
                {
                    new_channel = *it;
                    break;
                }
            }
        }
        return new_channel;
    }
}

bool Client::JOIN_existing_Channel(Client &client, const std::string& channel_name, const std::string &password, std::vector<Channel> &channels)
{
    for (std::vector<Channel>::iterator it = channels.begin() ; it != channels.end(); it++)
    {
        if(it->GetName() == channel_name)
        {
            // if(it->get_invite_only() && !isInvited(client))
            // {
            //     sendError(client, "473", channel_name, "Cannot join Channel (+i)");
            //     return true;
            // }

            if(it->get_has_password() && it->get_password() != password)
            {
                sendError(client, "475", channel_name, "Cannot join Channel (+k)");
                return true;
            }

            if(it->get_limit() && it->get_maxusers() <= it->Clients.size())
            {
                sendError(client, "471", channel_name , "Cannot join Channel (+l)");
                return true;
            }

            it->addUser(&client);
            notifyChannelJoin(*it , client);
            has_joined = true;

            return true;
        }
    } 
    return false;
}

Channel & Client::creating_new_Channel(Client &client, const std::string& channel_name, std::vector<Channel> &channels)
{
    std::cout << "Creating new channel " << channel_name << std::endl;
    Channel new_channel(channel_name);
    new_channel.addUser(&client);
    channels.push_back(new_channel);
    notifyChannelJoin(new_channel, client);
    std::cout << client.getnickname() << client.get_clientfd() << std::endl;
    has_joined = true;
    return new_channel;
}


void Client::notifyChannelJoin(Channel& channel, Client& client)
{
    std::cout << "Notifying channel " << channel.GetName() << " of new user " << client.getnickname() << std::endl;
    std::string join_message = ":";
    join_message += client.getPrefix();
    join_message += " JOIN ";
    join_message += channel.GetName();
    join_message += "\r\n";

    for (std::vector<Client*>::const_iterator it = channel.Clients.begin(); it != channel.Clients.end(); ++it)
    {
        send((*it)->get_clientfd(), join_message.c_str(), join_message.length(), 0);
    }

    if (!channel.get_topic().empty())
    {
        std::string topic_message = ":";
        topic_message += client.getPrefix();
        topic_message += " 332 ";
        topic_message += client.getnickname();
        topic_message += " ";
        topic_message += channel.GetName();
        topic_message += " :";
        topic_message += channel.get_topic();
        topic_message += "\r\n";

        send(client.get_clientfd(), topic_message.c_str(), topic_message.length(), 0);
    }
}

void Client::sendError(Client& client, const std::string& errorCode, const std::string& channel, const std::string& message)
{
    std::string errorMsg;

    if (errorCode == "403")
    {  // ERR_NOSUCHCHANNEL
        errorMsg = "403 " + client.getnickname() + " " + channel + " :No such channel";
    }
    else if (errorCode == "404")
    {  // ERR_CANNOTSENDTOCHAN
        errorMsg = "404 " + client.getnickname() + " " + channel + " :Cannot send to channel";
    }
    else if (errorCode == "405") 
    {  // ERR_TOOMANYCHANNELS
        errorMsg = "405 " + client.getnickname() + " " + channel + " :You have joined too many channels";
    }
    else if (errorCode == "471") 
    {  // ERR_CHANNELISFULL
        errorMsg = "471 " + client.getnickname() + " " + channel + " :Cannot join channel (+l)";
    }
    else if (errorCode == "473") 
    {  // ERR_INVITEONLYCHAN
        errorMsg = "473 " + client.getnickname() + " " + channel + " :Cannot join channel (+i)";
    }
    else if (errorCode == "474") 
    {  // ERR_BANNEDFROMCHAN
        errorMsg = "474 " + client.getnickname() + " " + channel + " :Cannot join channel (+b)";
    }
    else if (errorCode == "475") 
    {  // ERR_BADCHANNELKEY
        errorMsg = "475 " + client.getnickname() + " " + channel + " :Cannot join channel (+k)";
    }
    else if (errorCode == "406") 
    {  // ERR_NEEDMOREPARAMS
        errorMsg = "406 " + client.getnickname() + " " + message + " :Not enough parameters";
    }
    else if (errorCode == "421") 
    {  // ERR_UNKNOWNCOMMAND
        errorMsg = "421 " + client.getnickname() + " " + message + " :Unknown command";
    }
    else if (errorCode == "432") 
    {  // ERR_ERRONEUSNICKNAME
        errorMsg = "432 " + client.getnickname() + " " + message + " :Erroneus nickname";
    }
    else if (errorCode == "433") 
    {  // ERR_NICKNAMEINUSE
        errorMsg = "433 " + client.getnickname() + " " + message + " :Nickname is already in use";
    }
    else if (errorCode == "441") 
    {  // ERR_USERNOTINCHANNEL
        errorMsg = "441 " + client.getnickname() + " " + message + " :They aren't on that channel";
    }
    else if (errorCode == "442") 
    {  // ERR_NOTONCHANNEL
        errorMsg = "442 " + client.getnickname() + " " + message + " :You're not on that channel";
    }
    else if (errorCode == "461") 
    {  // ERR_NEEDMOREPARAMS
        errorMsg = "461 " + client.getnickname() + " " + message + " :Not enough parameters";
    }
    else if (errorCode == "462") 
    {  // ERR_ALREADYREGISTERED
        errorMsg = "462 " + client.getnickname() + " :You may not reregister";
    }
    else if (errorCode == "471") 
    {  // ERR_CHANNELISFULL
        errorMsg = "471 " + client.getnickname() + " " + channel + " :Cannot join channel (+l)";
    }
    else {
        errorMsg = errorCode + " :Unknown error";
    }

    // Sending the error message to the client
    send(client.get_clientfd(), errorMsg.c_str(), errorMsg.length(), 0);
}

