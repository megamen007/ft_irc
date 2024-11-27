/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboudrio <mboudrio@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 00:27:58 by mboudrio          #+#    #+#             */
/*   Updated: 2024/11/27 06:00:22 by mboudrio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int Server::Signal_status = 0;

void Server::Signal_Handler(int signum)
{
    (void)signum;

    std::cout << " Signal " << signum << "received. " << std::endl;
    Signal_status = signum; 
}

void Server::socket_creation()
{
    // 1 - socket creation 
    // socket(domaine , type , protocol);
    Serverfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (Serverfd < 0)
    {
        throw(std::runtime_error(" ---> Socket Creation stage failed "));
    }
}

sockaddr_in Server:: socket_infos()
{
    // 2 - defining server address  i can use setsockopt also for this 
    sockaddr_in serveradd;
    std::memset(&serveradd, 0, sizeof(serveradd)); // ensure initialisation and clean memory 
    serveradd.sin_family = AF_INET ; // specify addr family;
    serveradd.sin_port = htons(this->Port) ; // specify port number; htons is used to ensure that port numbers are represented in the correct byte order when transmitted over the network 
    // why order matters ? -->   When different computers communicate over a network, they need to agree on how multi-byte values (like port numbers and IP addresses) are represented. Since different computer architectures may use different byte orders, a common standard (network byte order) is used to ensure compatibility.
    serveradd.sin_addr.s_addr = INADDR_ANY ; // it is used when we dont want to bind our network to any particular IP and make it listen to all available IPs
    int enabling_reuse = 1;
    int infos_status = setsockopt(Serverfd, SOL_SOCKET, SO_REUSEADDR, &enabling_reuse, sizeof(enabling_reuse));
    if (infos_status < 0)
        throw(std::runtime_error(" ---> failed to make the local address reusable "));

    return serveradd;
}
void Server::socket_non_blocking()
{
    int non_blocking_status = fcntl(Serverfd, F_SETFL, O_NONBLOCK);
    if (non_blocking_status < 0)
    {
        throw(std::runtime_error(" ---> failed to make the socket non blocking "));
    }
}
void Server::socket_Binding(sockaddr_in serveradd)
{
    int bind_status = bind(Serverfd , (struct sockaddr*) &serveradd , sizeof(serveradd));
    if (bind_status < 0)
    {
        throw(std::runtime_error(" ---> Binding stage Failed ..."));
    }
}

void Server::socket_listening()
{
    // 4 - listen for incoming connections 
    int listening_status = listen(Serverfd ,  SOMAXCONN);
    if ( listening_status < 0)
    {
        throw(std::runtime_error(" ---> listening stage Failed ..."));
    }
}

void Server::server_socket_polling()
{
    struct pollfd Poll;

    Poll.fd = Serverfd;
    Poll.events = POLLIN; 
    Poll.revents = 0;

    fds.push_back(Poll);
}

void Server::client_socket_polling(int client_fd)
{
    struct pollfd NPoll;

    NPoll.fd = client_fd;
    NPoll.events = POLLIN;
    NPoll.revents = 0;
}

void Server::Launching_server(int port, std::string password)
{
    this->Port = port;
    this->Password = password;
    
    // Socket Operations :
    socket_creation();
    sockaddr_in freemon = socket_infos();
    socket_non_blocking();
    socket_Binding(freemon);
    socket_listening();
    server_socket_polling();

    std::cout << " Your Server Sir is in the Listening mode waiting for incoming connections  ... waiting to accept them" << std::endl;
    
    Server_cycle();
}

void Server::Server_cycle()
{
    while(!get_Signal_Status())
    {
        if((poll(fds.data(), fds.size(), -1) == -1) && get_Signal_Status() == false)
        {
            throw(std::runtime_error(" ---> poll stage failed "));
        }

        size_t i = 0;
        while( i < fds.size())
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == Serverfd)
                    socket_Accepting();
                else 
                    socket_receiving(fds[i].fd);
            }
            i++;
        }
    }
    // Close_filedescriptors();
}


void Server::socket_Accepting()
{
    Client *client = new Client();
    // client = getClient(fd);
    struct sockaddr_in clientadd;
    socklen_t len = sizeof(clientadd);

    int coming_fd = accept(Serverfd, (sockaddr *)&(clientadd), &len);

    if (coming_fd == -1)
        throw(std::runtime_error(" ---> accept stage failed "));

    int non_blocking_status = fcntl(coming_fd, F_SETFL, O_NONBLOCK);
    if (non_blocking_status < 0)
        throw(std::runtime_error(" ---> failed to make the socket non blocking "));

    // client_socket_polling(coming_fd);

    struct pollfd NPoll;

    NPoll.fd = coming_fd;
    NPoll.events = POLLIN;
    NPoll.revents = 0;
    client->setfd(coming_fd);
    client->setIPaddress(inet_ntoa(clientadd.sin_addr));
    Clients.push_back(client);
    fds.push_back(NPoll);
    
    std::cout << " ---> Client connected " << std::endl;
}


void Server::socket_receiving(int client_fd)
{
    char buffer[1024];
    Buffer Parser;
    Client *client = getClient(client_fd);
    memset(buffer, 0, sizeof(buffer));

    int r = recv(client_fd, buffer, sizeof(buffer), 0);
    RawData = buffer;
    if (r < 0) 
    {
        if (errno == EAGAIN || errno  == EWOULDBLOCK)
            return;
            
        std::cerr << " --> receiving stage failed ... " << std::endl;
        Remove_Client(client_fd);
        close(client_fd);

    }
    else if (r == 0)
    {
        std::cout << " --> Connection closed by Icaruis ..." << std::endl;
        close(client_fd);
    }
    else 
    {
        std::cout << " Received " << r << "  bytes ... " << std::endl;
        std::cout << " Received Data :  " << getRawData() << std::endl;
        std::cout << client->get_clientfd() << std::endl;
        
        if (!client->getlogedstatus())
            registerClient(client_fd, buffer);

        std::cout << "logo" << client->getlogedstatus() << std::endl;
        
        if (client->getlogedstatus())
            Parcing_and_Executing(client_fd, buffer, Parser);
    }
    
    if (std::string(buffer) == "exit") 
    {
        std::cout << "You requested to exit. Closing connection." << std::endl;
        close(client_fd);
        Remove_Client(client_fd);
    }
    
}



void Server::Parcing_and_Executing(int fd,std::string buffer, Buffer &Parser)
{
    Parser.Parcing_core(buffer);
    executing_commands(fd , Parser);
}

Channel *create_channel(Client *cl, std::string name, std::string pass)
{
    Channel *chan = new Channel(name, pass);
    std::cout << "wesh a jmi" << std::endl;
    std::cout << sizeof(chan) << std::endl;
    chan->addAdmin(cl);
    chan->addUser(cl, pass);
    chan->getMembers();
    // chan->get
    return chan;
}


void Server::addChannel(Channel *chan) {
    Channels.push_back(chan);
}


void Server::executing_commands(int fd, Buffer &Parser)
{   
    Client *client = getClient(fd);
    std::cout << Parser.get_cmd() <<std::endl; 
    if (Parser.get_cmd() == "JOIN")
    {
        std::cout << Parser.get_arg()[0] << std::endl;
        if (Parser.get_arg()[0] == '#') 
        { 
            std::cout << "ana 9na " << std::endl;
            Channel *chan = getChannel(Parser.get_arg());
            std::cout << client->getnickname() << " hiya smiya dyal l clinet li gay joini " << std::endl;
            if (chan == NULL)
            {
                std::cout << "ana 7na " << std::endl;
                chan = create_channel(client, Parser.get_arg(), Parser.get_msg());
                addChannel(chan);
            } 
            else 
            {
                std::cout << "ana bna " << std::endl;
                chan->addUser(client, Parser.get_msg());
            }
        }
        // } else {
        // //TODO : implement error hundling 
        // }
    } 
    else if (Parser.get_cmd() == "WHO")
    {
        std::cout << "dkkhalt hnaaa \n\n\n\n";
        Channel *chan = getChannel(Parser.get_arg());
        if (chan) {
            chan->WHO(client);
        } else {
            
        }
    }
    else if (Parser.get_cmd() == "KICK")
    {
        if (client->getoperatorstatus())
        {
             std::string target_nick = Parser.get_target();
            Client *target = findClientByNick(target_nick);
            Channel *chan = getChannel(Parser.get_arg());
                if (chan)
            chan->KICK(client, target, Parser.get_msg());
        }
        else
            std::cout << "U are not the father !" << std::endl;
    }
    else if (Parser.get_cmd() == "TOPIC")
    {
        if (client->getoperatorstatus())
        {
            Channel *chan = getChannel(Parser.get_arg());
            if (chan)
                chan->TOPIC(client, Parser.get_arg());
            else
            {
                std::cout << "Channel doesnt exist, " << std::endl;
            }
        }
        else
            std::cout << "U are not the father !" << std::endl;
    }
    else if (Parser.get_cmd() == "INVITE")
    {
        if (client->getoperatorstatus())
        { 
            std::string target_nick = Parser.get_target();
            Client *target = findClientByNick(target_nick);
            Channel *chan = getChannel(Parser.get_arg());
            if (chan)
             chan->INVITE(client, target);
            else
                std::cout << "Channel doesnt exist," << std::endl;
        }
        else
            std::cout << "U are not the father !" << std::endl;
    }
    else if (Parser.get_cmd() == "MODE")
    {
        if (client->getoperatorstatus())
        {
            Channel *chan = getChannel(Parser.get_arg());
            
            if (chan)
                chan->MODE(client, Parser.get_msg() , Parser.get_trg());
            else
                std::cout << "Channel doesnt exist," << std::endl;
        }
        else
            std::cout << "U are not the father !" << std::endl;
    }
    else if (Parser.get_cmd() == "PRIVEMSG")
    {
        std::string target_nick = Parser.get_target();
        Client *target = findClientByNick(target_nick);
        Channel *chan = getChannel(Parser.get_arg());
        if (chan)
        chan->PRIVMSG(client , target, Parser.get_prv_msg());
        else
                std::cout << "Channel doesnt exist," << std::endl;
    }
    else if (Parser.get_cmd() == "PART")
    {
        Channel *chan = getChannel(Parser.get_arg());
        if (chan)
            chan->PART(client , Parser.get_reason());
        else
            std::cout << "Channel doesnt exist," << std::endl;
    }           
}

Client* Server::findClientByNick(const std::string& nickname)
{
    for(size_t i = 0; i < Clients.size(); i++)
    {
        if (Clients[i]->getnickname() == nickname)
            return Clients[i];
    }
    return NULL;
}

