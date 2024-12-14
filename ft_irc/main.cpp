#include "Client.hpp"
#include "Channel.hpp"
#include "server.hpp"

int main(int ac, char **av)
{
    Server Excalibur ;
    if (ac != 3)
    {
        std::cerr << "Usage: " << av[0] << " <port number> <password> " << std::endl;
        return 1;
    }
    try
    {
        // list of signals that interrupt the irc server : 
        signal(SIGINT, Server::Signal_Handler);
        signal(SIGQUIT, Server::Signal_Handler);

        // Launching the Server:
        if (!Excalibur.Port_valid(av[1]))
        {
            std::cerr << " Your Port Number is Incorrect or Invalid " << std::endl;
            return 1;
        }
        else if (!*av[2])
        {
            std::cerr << " You didnt enter a password " << std::endl;
            return 1;
        }
        else if (std::strlen(av[2]) > 20)
        {
            std::cerr <<  "Your Password is More than 20 Character try a shorter version king " << std::endl;
            return 1;
        }
        std::cout << " your server is Launching sir wait a moment ..." << std::endl ;
        Excalibur.Launching_server(std::atoi(av[1]), av[2]);
        Excalibur.cleanupServer();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    std::cout << "Sadly we inform u that your Server Closed until another announcement .... be safe" << std::endl;
    return 0;
}