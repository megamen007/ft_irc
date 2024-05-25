#include "ircserv.hpp"

int main()
{
    Server Excalibur ;
    std::cout << " your server is Launching sir wait a moment ..." << std::endl ;
    try
    {
        // list of signals that interrupt the irc server : 
        signal(SIGINT, Server::Signal_Handler);

        //Launching the Server:
        Excalibur.Launching_server();
        
    }
    catch()
    {

    }
}