#include "ircserv.hpp"

void Server::Signal_Handler(int signum)
{
    (void)signum; // why ?

    std::cout << " Signal Received " << std::endl;
    Server::Signal_status = true;
}

void Server::remove_c_from_pollfd(int id)
{
    int i = 0 ;
    while(i < fds.size() )
    {
        if (fds[i].fd == id)
            fds.erase(fds.begin() + i);
        i++;
    }
}
void Server::remove_c_from_Vector(int id)
{
    int i = 0 ;
    while(i < Clients.size() )
    {
        if (Clients[i].get_clientfd() == id)
            Clients.erase(Clients.begin() + i);
        i++;
    }
}

void Server::close_all_clients()
{
    int i = 0;
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

void  Server::Remove_Client(int id)
{
    remove_c_from_pollfd(id);
    remove_c_from_Vector(id);
}

void Server::socket_creation()
{
    // 1 - socket creation 
    // socket(domaine , type , protocol);
    Serverfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (Serverfd < 0)
    {
        throw(std::runtime_error(" Socket Creation failed "));
    }
}

sockaddr_in Server::socket_infos()
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
        throw(std::runtime_error(" failed to make the local address reusable "));
    }
    return serveradd;
}
void Server::socket_non_blocking()
{
    int non_blocking_status = fcntl(Serverfd, F_SETFL, O_NONBLOCK);
    if (non_blocking_status < 0)
    {
        throw(std::runtime_error(" failed to make the socket non blocking "));
    }
}
void Server::socket_Binding(sockaddr_in serveradd)
{
    // 3 - Binding server socket 
    // bind function : The bind function in socket programming is used to associate a socket with a specific network address, such as an IP address and port number, on the local machine.
    // syntax : bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int bind_status = bind(Serverfd , (struct sockaddr*) &serveradd , sizeof(serveradd));
    if (bind_status < 0)
    {
        throw(std::runtime_error(" Bind Failed ..."));
    }
}

void Server::socket_listening()
{
    // 4 - listen for incoming connections 
    int listening_status = listen(Serverfd ,  SOMAXCONN);
    if ( listening_status < 0)
    {
        throw(std::runtime_error(" listen Failed ..."));
    }
}

void Server::server_socket_polling()
{
    struct pollfd Poll;

    Poll.fd = Serverfd;
    Poll.events = POLL_IN;
    Poll.revents = 0;

    fds.push_back(Poll);
}

void Server::client_socket_polling(int client_fd)
{
    struct pollfd NPoll;

    NPoll.fd = client_fd;
    NPoll.events = POLL_IN;
    NPoll.revents = 0;
    
    fds.push_back(NPoll);
}

void Server::socket_receiving(int client_fd)
{
    char buffer[1024] = {0};
    memset(buffer, 0, sizeof(buffer));
    int r = recv(client_fd, buffer, sizeof(buffer), 0);
    if (r < 0)
    {
        perror(" receiving Failed Boss ... ");
        Remove_Client(client_fd);
        close(client_fd);
    }
    else if (r == 0)
    {
        std::cout << " Connection closed by Icaruis ..." << std::endl;
        close(client_fd);
    }
    else 
    {
        std::cout << " Received " << r << "bytes ... " << std::endl;
        std::cout << " Received Data :  " << buffer << std::endl;
        Parcing_data_core(buffer);
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
std::string Server::extract_arg(std::string trimmed_data)
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

void Commands_errors(std::string cmd)
{
    std::vector <std::string> valid_Commands = {"KICK","INVITE","TOPIC","MODE"};

    if (std::find(valid_Commands.begin(), valid_Commands.end(), cmd) == valid_Commands.end())
        throw(std::runtime_error(" Invalid command : " + cmd));
}

int arguments_counter(std::string arg)
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

void Server::Arguments_errors(std::string cmd ,std::string arg)
{
    int x;
    x = arguments_counter(arg);
    if (x == 0)
        missing_arg_error(cmd);
    // if (x == 1)
    //     one_arg_errors(cmd, arg);
    // else 
    //     multiple_args_errors(cmd, arg);
}
void Server::missing_arg_error(std::string cmd)
{
    if (cmd.compare("KICK") == 0)
    {
        // Missing target user/channel argument.
        throw(std::runtime_error(cmd + "Missing target user/channel argument."));
    }
    else if (cmd.compare("TOPIC") == 0)
    {
        // Missing topic argument/channel argument.
        throw(std::runtime_error(cmd + "Missing topic argument / channel argument."));
    }
    else if (cmd.compare("INVITE") == 0)
    {
        // Missing target user/channel argument.
        throw(std::runtime_error(cmd + "Missing target user/channel argument."));
    }
    else if (cmd.compare("MODE") == 0)
    {
        // Missing target user/channel argument.
        throw(std::runtime_error(cmd + "Missing target user/channel argument."));
    }
}
void Server::checking_trimmed_data_errors(std::string trimmed_data)
{
    //example "  KICK     -p  -o"
    std::string cmd , arg;

    cmd =  extaract_cmd(trimmed_data);
    arg = trimming_raw_data(extract_arg(trimmed_data));
    Commands_errors(cmd);
    Arguments_errors(cmd , arg);
}

std::vector <std::string> Server::filling_msg_container(std::string trimmed_data)
{
    std::vector <std::string> eternal;
    std::string cmd , arg;

    cmd =  extaract_cmd(trimmed_data);
    arg = trimming_raw_data(extract_arg(trimmed_data));

    eternal.push_back(cmd);
    if (arguments_counter(arg) == 1)
        eternal.push_back(arg);
    else if (arguments_counter(arg) > 1)
    {
        int start = 0;
        int end = arg.find(' ');
        while(end != -1)
        {
            eternal.push_back(trimming_raw_data(arg.substr(start, end - start)));
            start = end + 1;
            end = arg.find(' ', start);
        }
        eternal.push_back(trimming_raw_data(arg.substr(start, end - start)));
    }
    return msg;
} 
void Server::creating_msg_container(std::string trimmed_data)
{
    msg =  filling_msg_container(trimmed_data);
}

// void Server::executing_commands()
// {
//     if(Operator_status == 1)
//     {
//         // operator priveleges :
//         if (msg[0].compare("KICK") == 0)
//             // kick_func();  
//         else if (msg[0].compare("INVITE") == 0) 
//             // invite_func();
//         else if (msg[0].compare("MODE") == 0) 
//             // mode_func(); 
//         else if (msg[0].compare("TOPIC") == 0)
//             // topic_func();
//         else if (msg[0].compare("JOIN") == 0) 
//         // normal User priveleges :
//             // join_func();
//         else if (msg[0].compare("PRIVEMSG") == 0) 
//             // privemsg_func();
//     }
//     else 
//     { 
//         // normal User priveleges :
//         if (msg[0].compare("JOIN") == 0) 
//             // join_func();
//         else if (msg[0].compare("PRIVEMSG") == 0) 
//             // privemsg_func()
//     }
// }
void Server::Parcing_data_core(char *buffer)
{
    std::string raw_data, trimmed_data ;
    raw_data = buffer;
    trimmed_data = trimming_raw_data(raw_data);
    checking_trimmed_data_errors(trimmed_data);
    creating_msg_container(trimmed_data);
    // roles_check();
    executing_commands();
}

void Server::socket_Accepting()
{
    Client Exodia;
    struct sockaddr_in clientadd;
    socklen_t len = sizeof(clientadd);

    int coming_fd = accept(Serverfd, (sockaddr *)&(clientadd), &len);

    if (coming_fd == -1)
    {
        throw(std::runtime_error(" accept failed "));
    }

    int non_blocking_status = fcntl(coming_fd, F_SETFL, O_NONBLOCK);
    if (non_blocking_status < 0)
    {
        throw(std::runtime_error(" failed to make the socket non blocking "));
    }

    client_socket_polling(coming_fd);

    Exodia.set_clientfd(coming_fd);
    Exodia.set_clientIP(inet_ntoa(clientadd.sin_addr));
    Clients.push_back(Exodia);
    
    std::cout << " Client connected " << std::endl;
}
void Server::Server_cycle()
{
    while(Server::Signal_status == false)
    {
        if((poll(&fds[0], fds.size(), -1) == -1) && Server::Signal_status == false)
        {
            throw(std::runtime_error(" poll failed "));
        }

        int i = 0;
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
    Close_filedescriptors();
}

void Server::Launching_server()
{
    this->Port = 8080;
    socket_creation();
    sockaddr_in freemon = socket_infos();
    socket_non_blocking();
    socket_Binding(freemon);
    socket_listening();
    server_socket_polling();

    std::cout << " Your Server Sir is in the Listening mode waiting for incoming connections ... " << std::endl;
    std::cout << " Waiting to accept Connections " << std::endl;

    Server_cycle();
}


// Brain storming :
// dont forget to delete the Serverfd in socket creation fn and replace it with serverfd that is declared in server class , and also remove it from function that can actually call it without the need of an argument related to them . 