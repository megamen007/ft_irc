/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 00:27:58 by mboudrio          #+#    #+#             */
/*   Updated: 2024/11/23 00:40:59 by otelliq          ###   ########.fr       */
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
    
    fds.push_back(NPoll);
}

void Server::Launching_server(int port, std::string password, Server &Excalibur)
{
    this->Port = port;
    this->Password = password;
    socket_creation();
    sockaddr_in freemon = socket_infos();
    socket_non_blocking();
    socket_Binding(freemon);
    socket_listening();
    server_socket_polling();

    std::cout << " Your Server Sir is in the Listening mode waiting for incoming connections  ... waiting to accept them" << std::endl;

    Server_cycle(Excalibur);
}

void Server::Server_cycle(Server &Excalibur)
{
    Client Exodia;
    
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
                    socket_Accepting(Exodia);
                else 
                    socket_receiving(fds[i].fd, Exodia, Excalibur);
            }
            i++;
        }
    }
    // Close_filedescriptors();
}


void Server::socket_Accepting(Client &client)
{
    // Client Exodia;
    struct sockaddr_in clientadd;
    socklen_t len = sizeof(clientadd);

    int coming_fd = accept(Serverfd, (sockaddr *)&(clientadd), &len);

    if (coming_fd == -1)
        throw(std::runtime_error(" ---> accept stage failed "));

    int non_blocking_status = fcntl(coming_fd, F_SETFL, O_NONBLOCK);
    if (non_blocking_status < 0)
        throw(std::runtime_error(" ---> failed to make the socket non blocking "));

    client_socket_polling(coming_fd);

    client.setfd(coming_fd);
    client.setIPaddress(inet_ntoa(clientadd.sin_addr));
    Clients.push_back(client);
    
    std::cout << " ---> Client connected " << std::endl;
}


void Server::socket_receiving(int client_fd, Client &client ,Server &Excalibur)
{
    char buffer[1024];
    Buffer Parser;
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
            //  I need to combine the registration system with Parcing and executing in a better way .
            std::cout << " Received " << r << "  bytes ... " << std::endl;
            std::cout << " Received Data :  " << getRawData() << std::endl;
            
            // Parser.Parcing_core(buffer);
            registerClient(client_fd, buffer);
            std::cout << " Client is registred " << std::endl;

            client.setnickname(n_name);
            client.setusername(u_name);
            client.setservername(s_name);
            client.setrealname(r_name);
            client.sethostname(h_name);
            client.setIPaddress(h_name);
            // Assuming this for hostname
            std::cout << "nickname" << client.getnickname() << std::endl;
            std::cout << "username" << client.getusername() << std::endl;
            std::cout << "hostname" << client.gethostname() << std::endl;
            std::cout << "servername" << client.getservername() << std::endl;
            // our client is registred .
            // if (registration_status)
            Parcing_and_Executing(client_fd,buffer,Parser, client, Excalibur);
        }
        if (std::string(buffer) == "exit") 
        {
            std::cout << "Client requested to exit. Closing connection." << std::endl;
            close(client_fd);
            Remove_Client(client_fd);
        }
    
}



void Client::executing_commands(__unused int fd, std::string Cmd, Buffer &Parser , Client &client, Server &Excalibur)
{
//     if(Operator_status == 1)
//     {
//         // operator priveleges :
//         if (Cmd.compare("KICK") == 0 || Cmd.compare("kick") == 0)
//             // kick_func();
//         else if (Cmd.compare("INVITE") == 0 || Cmd.compare("invite") == 0)
//             // invite_func();
//         else if (Cmd.compare("MODE") == 0 || Cmd.compare("mode") == 0)
//             // mode_func();
//         else if (Cmd.compare("TOPIC") == 0 || Cmd.compare("topic") == 0)
//             // topic_func();  
       if (Parser.get_cmd().compare("JOIN") == 0 || Parser.get_cmd().compare("join") == 0)
             client.JOIN(client, Cmd, Parser , Excalibur);
//         else if (Cmd.compare("PRIVEMSG") == 0 || Cmd.compare("privemsg") == 0)
//             // privemsg_func();

//             // pass_func();
//     }
//     else
//     {
//         // normal User priveleges :
//         if (Cmd.compare("JOIN") == 0 || Cmd.compare("join") == 0)
//             // join_func();
//         else if (Cmd.compare("PRIVEMSG") == 0  || Cmd.compare("privemsg") == 0)
//             // privemsg_func()
//         else if (Cmd.compare("NICK") == 0 || Cmd.compare("nick")  == 0)
//             // nick_func();
//         else if (Cmd.compare("USER") == 0  || Cmd.compare("user") == 0)
//             // user_func();
//         else if (Cmd.compare("PASS") == 0 || Cmd.compare("pass") == 0)
//             // pass_func();
//     }
}



void Server::Parcing_and_Executing(int client_fd, std::string buffer,Buffer &Parser, Client &client ,Server &Excalibur)
{
    // (void)client_fd;
    Parser.Parcing_core(buffer);
    // OTHMAN PART ( where to execute the list of Command depending on the Parced Buffer)
    client.executing_commands(client_fd , buffer, Parser , client, Excalibur); // need to start coding nick , pass , user , join and creating chanells ;
}


