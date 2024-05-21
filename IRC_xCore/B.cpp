#include <iostream>
#include <sys/socket.h> // contains definitions of function and structures , socket(), bind(), listen(), and accept(). Defines the sockaddr structure, which is a generic structure for all socket addresses. 
#include <netinet/in.h> // Contains definitions for Internet Protocol family , Defines the sockaddr_in structure, used for IPv4 addresses. , Contains macros like INADDR_ANY and functions like htons() .
#include <cstring>
#include <unistd.h>

// client side ;
// creat socket 
// connect the socket 
// send and receive data 

// server side 
// create socket 
// bund socket 
// listen
// accept 
// send and receive data 

int main ()
{ 
    // 1 - socket creation 
    int server_fd ;
    // socket(domaine , type , protocol);
    server_fd =  socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror(" Socket Creation failed ");
        return 1;
    }

    // 2 - defining server address  i can use setsockopt also for this 
    sockaddr_in serveradd;
    std::memset(&serveradd, 0, sizeof(serveradd)); // ensure initialisation and clean memory 
    serveradd.sin_family = AF_INET ; // specify addr family;
    serveradd.sin_port = htons(8080) ; // specify port number; htons is used to ensure that port numbers are represented in the correct byte order when transmitted over the network 
    // why order matters ? -->   When different computers communicate over a network, they need to agree on how multi-byte values (like port numbers and IP addresses) are represented. Since different computer architectures may use different byte orders, a common standard (network byte order) is used to ensure compatibility.
    serveradd.sin_addr.s_addr = INADDR_ANY ; // it is used when we dont want to bind our network to any particular IP and make it listen to all available IPs

    // 3 - Binding server socket 
    // bind function : The bind function in socket programming is used to associate a socket with a specific network address, such as an IP address and port number, on the local machine.
    // syntax : bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    if (bind( server_fd, (struct sockaddr*) &serveradd , sizeof(serveradd)) < 0)
    {
        perror(" Bind Failed ...");
        close(server_fd); //close the socket and release any associated resources ; 
        return 1;
    }

    // 4 - listen for incoming connections 
    if (listen(server_fd ,  5) < 0)
    {
        perror(" listen Failed ...");
        close(server_fd);
        return 1;
    }

    std::cout << " Your Server Sir is in the Listening mode waiting for incoming connections ... " << std::endl;
    while(true)
    while(true)
    {
    // 5 - Accepting a client Connection
    int client_socket = accept(server_fd, nullptr, nullptr);
    if (client_socket < 0)
    {
        perror("accepting Failed ...");
        close(server_fd);
        return 1;
    }

    std::cout << " Client connected" << std::endl;

    
    // receiving & sending data 
    const void  *msg = " Hello from The Eternals ...";
    int s = send(client_socket, msg , sizeof(msg), 0);
    if (s < 0)
    {
        perror("receiving Failed Boss ...");
        close(client_socket);
        return 1;
    }
    else 
    {
        std::cout << s <<  " bytes was sent ..." << std::endl;
    }

    char buffer[1024] = {0};
    int r = recv(client_socket, buffer, sizeof(buffer), 0);
    if (r < 0)
    {
        perror(" receiving Failed Boss ... ");
        close(client_socket);
        return 1;
    }
    else if (r == 0)
    {
        std::cout << " Connection closed by Icaruis ..." << std::endl;
        close(client_socket);
        return 1;
    }
    else 
    {
        std::cout << " Received "<< r << "bytes From DR DOOM ... " << std::endl;
        std::cout << " Celestial sends to u this Holly Message  " << buffer << std::endl;
    }
        close(client_socket);
    }
    
    //close socket
    close (server_fd);
    return 0;
}