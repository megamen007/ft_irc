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

    // Sending connection request
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return 1;
    }

    while (true) 
    {
        // Sending data
        std::string msg;
        std::cout << "Enter message to send (or 'exit' to quit): ";
        std::getline(std::cin, msg);
        
        if (msg == "exit")
            break;

        int s = send(client_fd, msg.c_str(), msg.length(), 0);
        if (s < 0) {
            perror("Send failed");
            close(client_fd);
            return 1;
        } 
        else
            std::cout << s << " bytes were sent." << std::endl;
    }
    close(client_fd);

}
