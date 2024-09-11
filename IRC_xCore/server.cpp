/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboudrio <mboudrio@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 00:27:58 by mboudrio          #+#    #+#             */
/*   Updated: 2024/09/11 05:25:02 by mboudrio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ircserv.hpp"

// Client setters & getters

Client::Client()
{
    this->nickname = "";
    this->clientIP = "";
    this->clientfd = -1;
    this->Operator_status = false;
    this->username = "";
}
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

Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->nickname = src.nickname;
        this->username = src.username;
        this->clientfd = src.clientfd;
        this->clientIP = src.clientIP;
        this->Operator_status = src.Operator_status;
    }
    return *this;
}



/// ///////////////////////////////////////


Server &Server::operator=(Server const &src)
{
    if (this != &src)
    {
        this->Port = src.Port;
        this->Serverfd = src.Serverfd;
        this->Pass = src.Pass;
        this->Clients = src.Clients;
        // this->Channels = src.Channels;
        this->fds = src.fds;
    }
    return *this;
}

// server setters :

void Server::setFd(int fd)
{
    this->Serverfd = fd;
}

void Server::setPassword(std::string password)
{
    this->Pass = password;
}

void Server::setPort(int port)
{
    this->Port = port; 
}

void Server::AddClient(Client newClient)
{
    this->Clients.push_back(newClient);
}
// void Server::AddChannel(Channel newChannel)
// {
//     this->channels.push_back(newChannel);
// }
// getters 

// void Server::Signal_Handler(int signum)
// {
//     (void)signum; // why ?

//     std::cout << " Signal Received " << std::endl;
//     Server::Signal_status = true;
// }

void Server::remove_c_from_pollfd(int id)
{
    size_t i = 0 ;
    while(i < fds.size() )
    {
        if (fds[i].fd == id)
            fds.erase(fds.begin() + i);
        i++;
    }
}
void Server::remove_c_from_Vector(int id)
{
    size_t i = 0 ;
    while(i < Clients.size() )
    {
        if (Clients[i].get_clientfd() == id)
            Clients.erase(Clients.begin() + i);
        i++;
    }
}
void  Server::Remove_Client(int id)
{
    remove_c_from_pollfd(id);
    remove_c_from_Vector(id);
}

void Server::close_all_clients()
{
    size_t i = 0;
    while( i < Clients.size())
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
    {
        throw(std::runtime_error(" ---> failed to make the local address reusable "));
    }
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
    // 3 - Binding server socket 
    // bind function : The bind function in socket programming is ued to associate a socket with a specific network address, such as an IP address and port number, on the local machine.
    // syntax : bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
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
    Poll.events = POLLIN;  // ->=
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

void Server::socket_receiving(int client_fd)
{
    char buffer[1024] = {0}; // to check cpp 98 ;
    memset(buffer, 0, sizeof(buffer));
    int r = recv(client_fd, buffer, sizeof(buffer), 0); 
    std::cout << " receive state : " << r << std::endl;
    if (r < 0) 
    {
        perror(" --> receiving stage failed ... ");
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
        std::cout << " Received Data :  " << buffer << std::endl;
        Parcing_data_core(client_fd,buffer);
    }
    
}

std::string Server::trimming_raw_data(std::string raw_data)
{
    int start = 0;
    int end = raw_data.size();

    while (start < end && raw_data[start] == ' ')
    start++;

    while (end > start && raw_data[end - 1] == ' ')
    end--;

    return raw_data.substr(start, end - start);  
}

std::string Server::extaract_cmd(std::string trimmed_data)
{
    int start = 0;
    int end = trimmed_data.size();
    

    while (start < end && trimmed_data[start] == ' ')
    start++;
    int middle = start;
    
    while (middle < end && trimmed_data[middle] != ' ' )
    middle++;

    return trimmed_data.substr(start, middle - start); 
}
std::string Server::extract_arg_v2(std::string trimmed_data)
{
    int start = 0;
    int end = trimmed_data.size();
    
    while (start < end && trimmed_data[start] == ' ')
        start++;
    int middle = start;
    
    while (middle < end && trimmed_data[middle] != ' ' )
        middle++;
    
    return trimmed_data.substr(middle, end - middle);
}

std::string Server::extract_arg(std::string trimmed_data)
{
    int start = 0;
    int end = trimmed_data.size();
    

    while (start < end && trimmed_data[start] != '-')
        start++;

    while (start < end && trimmed_data[start] == ' ')
        start++;
    int middle = start;
    
    while (middle < end && trimmed_data[middle] != ' ' )
        middle++;
    
    return trimmed_data.substr(middle, end - middle);
}
// " KICK  -i fbvd fvdvds fsbvsfbv"
std::string Server::extract_flags(std::string trimmed_data)
{
    int start = 0;
    int end = trimmed_data.size();

    while (start < end && (trimmed_data[start] != '-' && trimmed_data[start] != '+'))
    {

        std::cout << " start  == " <<  start << std::endl ;
        std::cout << " start  == " <<  trimmed_data[start] << std::endl ;
        start++;
    }
    
    // if (trimmed_data[start] == '-')
    //         flags_status = 0;
    // else if (trimmed_data[start] == '+')
    //         flags_status = 1;

       std::cout << " start  == " <<  start << std::endl ;
        std::cout << " start  == " <<  trimmed_data[start] << std::endl ;

    // Check if start is at the end or if the next character is not within valid flags
    std::cout << " start - 1 == " <<  trimmed_data[start - 1] << std::endl ;
    std::cout << " start + 1 == " <<  trimmed_data[start + 1] << std::endl ;
    std::cout << " trimmed_data[start + 1]  == " <<  trimmed_data[start + 1] << std::endl ;


    if (start == end || (trimmed_data[start + 1] != 'i' && trimmed_data[start + 1] != 't' &&
                         trimmed_data[start + 1] != 'o' && trimmed_data[start + 1] != 'k' &&
                         trimmed_data[start + 1] != 'l'))
    {
        return "Invalid flag"; // dont forget the case when u will face an empty flags in some commands .
    }
    else 
    {
        return trimmed_data.substr(start, 2); // Extract the dash and the flag character
    }
}

void Server::Commands_errors(std::string& cmd)
{
    const char* validCommands[] = {"KICK","INVITE","TOPIC","MODE","PASS","USER","PRIVEMSG","JOIN","NICK"};
    const std::vector <std::string> valid_Commands (validCommands, validCommands + 9);

    if (std::find(valid_Commands.begin(), valid_Commands.end(), cmd) == valid_Commands.end())
        std::cout << " the command that u write is incorrect , please rewrite ur command  "<< std::endl;

    else 
        std::cout << " clean grammar , good boy " << std::endl;
}

int Server::arguments_counter(std::string arg)
{
    int start = 0, counter = 0;
    int end = arg.size();

    while(start < end  && arg[start] == ' ')
        start++;

    while (start < end )
    {
        counter++;
        while(start < end && arg[start] != ' ')
        start++;
        while(start < end  && arg[start] == ' ')
        start++;
    }
    return counter;
}


void Server::missing_flag_error(std::string cmd, std::string arg)
{
    if ((cmd.compare("MODE") == 0) && (arg.compare("-i") || arg.compare("-t") || arg.compare("-k") || arg.compare("-o") || arg.compare("-l")) == 0)
        std::cout << " this command necessary need some certain flag like : -i , -t , -k , -o, -l" << std::endl;
}

void Server::one_flag_errors(std::string cmd)
{
    if (cmd.compare("TOPIC") == 0)
        std::cout << " this command necessary doesn't need any flags " << std::endl;
    else if (cmd.compare("KICK") == 0)
        std::cout << " this command necessary doesn't need any flags " << std::endl;
    else if (cmd.compare("NICK") == 0)
        std::cout << " this command necessary doesn't need any flags " << std::endl;
    else if (cmd.compare("PASS") == 0)
        std::cout << " this command necessary doesn't need any flags " << std::endl;
    else if (cmd.compare("USER") == 0)
        std::cout << " this command necessary doesn't need any flags " << std::endl;
    else if (cmd.compare("JOIN") == 0)
        std::cout << " this command necessary doesn't need any flags " << std::endl;
    else if (cmd.compare("PRIVEMSG") == 0)
        std::cout << " this command necessary doesn't need any flags " << std::endl;
    else if (cmd.compare("INVITE") == 0)
        std::cout << " this command necessary doesn't need any flags " << std::endl;
}
void Server::Arguments_errors(std::string cmd ,std::string arg, std::string flags)
{
    int x;
    x = arguments_counter(flags);
    std::cout << x << std::endl;
    if (x == 0)
        missing_flag_error(cmd , arg);
    if (x == 1)
        one_flag_errors(cmd);
}
void Server::flags_errors(std::string cmd, std::string flags)
{
    std::cout << flags << std::endl ;
    std::cout << cmd << std::endl;
        if ( (flags.compare("Invalid flag") == 0) && (cmd.compare("MODE") == 0)) 
            std::cout << " the flags that u enter doesn't match the available flag list , please try another flag " << std::endl;
        else if ((flags.compare("Invalid flag" ) == 0) && ((cmd.compare("KICK") == 0 ) || (cmd.compare("JOIN") == 0 ) ||
        (cmd.compare("USER") == 0 ) || (cmd.compare("PASS") == 0 ) || (cmd.compare("TOPIC") == 0 ) || (cmd.compare("PRIVEMSG") == 0 ) ||
        (cmd.compare("MODE") == 0 ) || (cmd.compare("NICK") == 0 )))
            std::cout << " the flag u mentioned doesn't seems compatible with the Command that u entered , please retry with another compatible Commande " << std::endl;
}
void Server::checking_trimmed_data_errors(std::string trimmed_data)
{
    std::string cmd , arg, flags;

    std::cout << " ---> trimmed_data : " << trimmed_data << std::endl ;

    cmd =  extaract_cmd(trimmed_data);
    std::cout << " cmd : " << cmd << std::endl;
    flags = trimming_raw_data(extract_flags(trimmed_data));
    std::cout << " flags : " << flags << std::endl;
    if (flags.compare("Invalid flag") == 0)
        arg = trimming_raw_data(extract_arg_v2(trimmed_data));
    else if (!(flags.compare("Invalid flag") == 0))
        arg = trimming_raw_data(extract_arg(trimmed_data));
    std::cout << " args : " << arg << std::endl;
    std::cout << " flags_status : " << flags_status << std::endl; 
    Commands_errors(cmd);
    Arguments_errors(cmd , arg , flags);
    flags_errors(cmd , flags);
}

// void Server::executing_commands(int fd, std::string Cmd)
// {
//     if(Operator_status == 1)
//     {
//         // operator priveleges :
//         if (msg[0].compare("KICK") == 0 || msg[0].compare("kick") == 0)
//             // kick_func();  
//         else if (msg[0].compare("INVITE") == 0 || msg[0].compare("invite") == 0) 
//             // invite_func();
//         else if (msg[0].compare("MODE") == 0 || msg[0].compare("mode") == 0) 
//             // mode_func(); 
//         else if (msg[0].compare("TOPIC") == 0 || msg[0].compare("topic") == 0)
//             // topic_func();
//         else if (msg[0].compare("JOIN") == 0 || msg[0].compare("join") == 0) 
//             // join_func();
//         else if (msg[0].compare("PRIVEMSG") == 0 || msg[0].compare("privemsg") == 0) 
//             // privemsg_func();
//         else if (msg[0].compare("NICK") == 0 || msg[0].compare("nick")  == 0) 
//             // nick_func(); 
//         else if (msg[0].compare("USER") == 0  || msg[0].compare("user") == 0)
//             // user_func();
//         else if (msg[0].compare("PASS") == 0 || msg[0].compare("pass") == 0) 
//             // pass_func();
//     }
//     else 
//     { 
//         // normal User priveleges :
//         if (msg[0].compare("JOIN") == 0 || msg[0].compare("join") == 0) 
//             // join_func();
//         else if (msg[0].compare("PRIVEMSG") == 0  || msg[0].compare("privemsg") == 0) 
//             // privemsg_func()
//         else if (msg[0].compare("NICK") == 0 || msg[0].compare("nick")  == 0) 
//             // nick_func(); 
//         else if (msg[0].compare("USER") == 0  || msg[0].compare("user") == 0)
//             // user_func();
//         else if (msg[0].compare("PASS") == 0 || msg[0].compare("pass") == 0) 
//             // pass_func();
//     }
// }

void Server::Parcing_data_core(int client_fd, std::string buffer)
{
    std::string raw_data, trimmed_data ;
    raw_data = buffer;
    trimmed_data = trimming_raw_data(raw_data);
    checking_trimmed_data_errors(trimmed_data);
    // creating_msg_container(trimmed_data);
    // executing_commands(client_fd , trimmed_data); // need to start coding nick , pass , user , join and creating chanells ;
}

void Server::socket_Accepting()
{
    Client Exodia;
    struct sockaddr_in clientadd;
    socklen_t len = sizeof(clientadd);

    int coming_fd = accept(Serverfd, (sockaddr *)&(clientadd), &len);

    if (coming_fd == -1)
        throw(std::runtime_error(" ---> accept stage failed "));

    int non_blocking_status = fcntl(coming_fd, F_SETFL, O_NONBLOCK);
    if (non_blocking_status < 0)
        throw(std::runtime_error(" ---> failed to make the socket non blocking "));

    client_socket_polling(coming_fd);

    // Exodia.set_clientfd(coming_fd);
    // Exodia.set_clientIP(inet_ntoa(clientadd.sin_addr));
    Clients.push_back(Exodia);
    
    std::cout << " ---> Client connected " << std::endl;
}
void Server::Server_cycle()
{
    while(Server::Signal_status == false)
    {
        if((poll(fds.data(), fds.size(), -1) == -1) && Server::Signal_status == false)
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

void Server::Launching_server(int port, std::string password)
{
    this->Port = port;
    this->Pass = password;
    socket_creation();
    sockaddr_in freemon = socket_infos();
    socket_non_blocking();
    socket_Binding(freemon);
    socket_listening();
    server_socket_polling();

    std::cout << " Your Server Sir is in the Listening mode waiting for incoming connections  ... waiting to accept them" << std::endl;

    Server_cycle();
}

bool Port_valid(std::string port)
{
    if (port.find_first_not_of("0123456789") == std::string::npos)
        return false;
    int portnum = std::atoi(port.c_str());
    if (portnum >= 1024 && portnum <= 65535)
        return true;
    else
        return false;
}

// Authentification Commands :
// PASS :

void Server::Pass_func(int fd, std::string cmd)
{
      Client *clio =  get_clientfd(fd);  
}
int main(int ac, char **av)
{
    Server Excalibur ;
    if (ac != 3)
    {
        std::cout << "Usage: " << av[0] << " <port number> <password> " << std::endl;
    }
    std::cout << " your server is Launching sir wait a moment ..." << std::endl ;
    try
    {
        // list of signals that interrupt the irc server : 
        // signal(SIGINT, Server::Signal_Handler);

        //Launching the Server:
        if (!Port_valid(av[1]))
            std::cout << " Your Port Number is Incorrect or Invalid " << std::endl;
        else if (!*av[2])
            std::cout << " You didnt enter a password " << std::endl;
        else if (std::strlen(av[2]) > 20)
            std::cout <<  "Your Password is More than 20 Character try a shorter version king " << std::endl;
        Excalibur.Launching_server(std::atoi(av[1]), av[2]);
        
    }
    catch(const std::exception& e)
    {
        // Excalibur.Close_filedescriptors();
        std::cerr << e.what() << std::endl;
    }

    std::cout << "Saddly we inform u that your Server Closed until another announcement be safe .. " << std::endl ;
}

// Brain storming :
// dont forget to delete the Serverfd in socket creation fn and replace it with serverfd that is declared in server class , and also remove it from function that can actually call it without the need of an argument related to them . 
