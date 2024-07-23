#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

int main ()
{
    // creating socket 
    int client_fd = socket (AF_INET , SOCK_STREAM , 0);

    // specfying the address
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // sending connection request 
    connect(client_fd, (struct sockaddr* )& server_addr, sizeof(server_addr));

    //sending & receiving data 
    const char  *msg = " Hello ";
    std::cout << " mojo  jojo " << std::endl;
    int s = send(client_fd, msg , strlen(msg), 0);
    if (s < 0)
    {
        perror(" receiving Failed Boss ...");
        close(client_fd);
        return 1;
    }
    else 
    {
        std::cout << s <<  " bytes was sent ..." << std::endl;
    }

    char buffer[1024] = "dfgfbesbhter";
    int r = recv(client_fd, buffer, sizeof(buffer), 0);
    if (r < 0)
    {
        perror(" receiving Failed Boss ... ");
        close(client_fd);
        return 1;
    }
    else if (r == 0)
    {
        std::cout << " Connection closed by Icaruis ..." << std::endl;
        close(client_fd);
        return 1; 
    }
    else 
    {
        std::cout << " Received "<< r << "bytes From Celestial ... " << std::endl;
        std::cout << " Doctor DOOM sends to u this Holly Message ..." << buffer << std::endl;
    }

    close(client_fd);
    return 0;
}