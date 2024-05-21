// understanding IRC (Internet Relay Chat) concepts :

// Goals : understanding and exploring computer networking and communication protocols .



// so the project will be devided to two big parts those parts are :

// $ Understanding every theorie aspect & concepts about ft-irc projects .

//------- Main Points ---------

// --Sockets Concept--

// 1--Definition--
// Sockets provide a standard interface between network and application .

// 2--Types of Sockets--

// some common types :

// #Stream Sockets (TCP):
// -->Stream sockets are connection-oriented sockets that use the Transmission Control Protocol (TCP).
// -->They provide reliable, bidirectional, and byte-stream communication between the client and server.
// -->TCP ensures that data sent from one end is received in the same order at the other end, and it handles retransmission of lost packets and flow control.

// #Datagram Sockets (UDP):
// -->Datagram sockets are connectionless sockets that use the User Datagram Protocol (UDP).
// -->They provide unreliable, connectionless communication between the client and server.
// -->UDP does not guarantee delivery or order of packets, and there is no connection setup phase like in TCP.

// #Raw Sockets:
// Raw sockets allow direct access to network protocols at the IP layer.
// They enable the creation of custom packets and manipulation of headers, which is useful for tasks like network scanning, packet sniffing, and protocol implementation.

// #Unix Domaine Sockets:
// Unix domain sockets are used for communication between processes on the same Unix or Unix-like system.
// They use file system APIs for communication and do not involve network communication.
// They are generally faster and have lower overhead compared to network sockets.

// #Sequenced Packet Sockets:
// Sequenced packet sockets are a reliable, connection-oriented socket type that preserves message boundaries.
// They provide a combination of features from both stream and datagram sockets, offering reliable delivery with explicit boundaries between messages.

//----> we want also to focus on Stream and Datagram Sockets beacause they are commonly considerd the most known and widely used types of sockets in networking beacuse of this reasons :
// Reliability(Stream) vs. Simplicity(Datagram) .
// Common Internet Protocols .
// Support Across Programming Languages .
// Stability and flexibility .

// 3--TCP/IP Connection--

// 3-1--Definition--
// A reliable byte-stream connection between two computers .
// Most commonly used in a client/server fashion . In a nutshell the server listens on a well-known port
// the client connect to that port , once connection is established , either side can write data into the connection
// where it becomes available for the other side to read 

// 3-2 TCP/IP & Sockets API 
// The difference between TCP/IP connection and the sockets API lies in their respective roles and layers within the networking stack:
// while TCP/IP governs the transmission of data over networks and operates at the transport layer , the sockets API provides a programming interface 
// for applications to utilize the networking capabilities provided by the operating system, including the ability to establish TCP/IP connections.

// that was the resume of an Sockets API that represent the connection using file descriptor .
// let`s delve in it .

// 4--the Sockets API--

// 4-1--what is an API ?
// An API, or Application Programming Interface, is essentially a set of rules, protocols, and tools for building software applications. 
// It defines how software components should interact with each other, 
// enabling developers to create applications that leverage the functionality provided by other software components or services.

// 4-2--what is a Socket API ?
// refers to the set of functions and data structures provided by an operating system or programming library for creating and interacting with network sockets.
// Network sockets are endpoints for communication between two machines over a network,
// and the socket API provides a standardized way for applications to establish connections, send and receive data, and manage network communication.

// 4-3--Socket API Steps ?
// The socket API typically includes functions for tasks such as:

// 4-3-1--Creating and initializing sockets :
// to create a socket, you typically use the socket() function . This function returns a socket descriptor .
// and returns  -1 in case of an errors (u can look at the errors types in tha man page "man 2 socket / man socket ") and set the global variable errno on failure .
// socket descriptor is a unique identifier for the socket .
// The socket() function takes  3 parameters : | syntax:   socket(family, type, protocol) .
// -->family/domaine  : AF_INET for IPv4 or AF_INET6 for IPv6 .
// -->type : SOCK_STREAM for TCP or SOCK_DGRAM for UDP .
// -->protocol : not used for internet sockets so we assign 0 to it .

// 4-3-2--Binding a Server socket :
// Before a socket can be used to send or receive data, it must be bound to a specific network address and port on the local machine.
// Bind a socket to a port on a network interface .
// Needed to run servers on a well-known port that's not generally used on clients, since typically don’t care which port used .
// This is typically done using the bind () function () , which takes 3 parametres : | synatx: bind(fd, addr, addrlen) .
// -->fd : the socket descriptor .
// -->the local address structure (it contains the IP address and port number).
// -->size of the address structure .
// additional infos about bind() :
// it`s actually bind a name to a socket beacuse when we create a socket using socket it exists in a name space (address family) but
// has no name assigned.  bind() requests that address be assigned to the socket.
// Return Value : succes case it return 0, in failure return -1;
// Binding allows the operating system to route incoming data packets to the appropriate socket based on the destination address.

// 4-3-3--Listening for Connections :
// Server sockets, which are used to accept incoming connections from clients, must be placed in a listening state.
// This is accomplished by calling the listen() function . which takes 2 parametres : | synatx: (listen(fd, backlog) .
// -->fd : the socket descriptor .
// -->backlog : parameter defines the maximum length for the queue of pending connections .
// Return Value : return 0 in succes  case, but in failure return -1;

// 4-3-4--Accept Connections (Server Only) :
// When a client tries to connect to the server, the server socket must accept the incoming connection request .
// so simply this how accept() :
// 1-function work , 2-Extracts the First Connection Request , 3-Creates a New Socket , 4-Allocates a New File Descriptor
// 5-Blocking Behavior : ( If there are pending connections in the queue when accept() is called, it immediately extracts the first connection and returns.
// However, if there are no pending connections in the queue and the socket is not set to non-blocking mode,
// accept() blocks the caller. This means that the program will wait at the accept() call until a new connection arrives.)
// 6-Non-blocking Behavior : (If the socket is set to non-blocking mode and there are no pending connections in the queue when accept() is called, it returns an error instead of blocking.
// Non-blocking mode allows the program to continue execution even if there are no pending connections, and the accept() call returns immediately with an error indication.)
// 7-Accepted Socket Usage 8-Original Socket Remains Open .
// The server calls the accept() | syntax:  accept(fd, struct sockaddr *address, socklen_t *addresslen)
// -->fd : the socket descriptor .
// -->*address : A pointer to a struct sockaddr structure where the address of the connecting client will be stored. 
// This allows the server to identify the IP address and port number of the client that initiated the connection.
// -->*addresslen : A pointer to a socklen_t variable that specifies the size of the addr structure. Upon return, 
// it will be updated with the actual size of the address stored in addr.
// Return Value : The call returns -1 on error and the global variable errno is set to indicate the error.  
// If it succeeds, it returns a non-negative integer that is a descriptor for the accepted socket.

// 4-3-5--Connect to a Server (Client Only) :
// client sockets initiate connections to remote servers by calling the connect() function .
// enerally, stream sockets may successfully connect() only once  datagram sockets may use connect() multiple times to change their associ-ation.
// Datagram sockets may dissolve the association by callingdisconnectx(2), or by connecting to an invalid address, such as a null address
// syntax : connect(int fd , const struct sockaddr *address, socklen_t address_len)
// --> fd : the socket descriptor .
// --> *address : Pointer to a struct sockaddr .
// --> address_len : Size of the struct in bytes .
// Return Value : Upon successful completion, a value of 0 is returned.
// Otherwise, a value of -1 is returned and the global integer variable errno is set to indi-cate the error.
// Upon a successful connection the client socket is ready to send a receive data withe the server .

// 4-3-6--Sending & receiving :
// Once a connection is established, both the client and server can exchange data using the send() and recv() functions. 
// sending : are used to transmit a message to another socket.  send() may be used only when the socket is in a connected state,
// while sendto() and sendmsg() may be used at any time.
// syntax : send(int socket, const void *buffer, size_t length, int flags);
// Return Value : pon successful completion, the number of bytes which were sent is returned.  Otherwise, -1 is returned and the global variable errno is set to indicate the error.
// receiving : are used to receive messages from a socket, and may be used to receive data on a socket whether or not
// it is connection-oriented.
// synatx : recv(int socket, void *buffer, size_t length, int flags);
// Return Value : These calls return the number of bytes received, or -1 if an error occurred.
// For TCP sockets, the return value 0 means the peer has closed its half side of the connection.
// so in conclusion --> the recv() function can be used in both connected and unconnected states, unlike the send() function, which typically works only in a connected state.

// 4-3-7--closing the socket :
// When a socket is no longer needed, it should be closed to release associated resources and properly clean up the connection.
// Closing a socket is done using the close() function, which takes the socket descriptor as a parameter.
// After closing the socket, the descriptor is no longer valid, and any further attempts to use it will result in an error.

// 4-4--Some additional informations : 
// Must specify the address and port when calling bind() or connect() .
// The address can be either IPv4 or IPv6 .
// Could be modelled in C as a union, but the designers of the sockets API chose to use a number of structs,
// and abuse casting instead .
// 4-4-1--struct sockaddr :
//                         struct sockaddr {
//                          uint8_t sa_len;
//                          sa_family_t sa_family;
//                          char sa_data[22];
//                         };
// Addresses specified via struct sockaddr .
// Has a data field big enough to hold the largest address of any family .
// Plus sa_len and sa_family to specify the length and type of the address .
// Treats the address as an opaque binary string .
// two variations exist for IPv4 & IPv6 addresses .
// 4-4-2--struct sockaddr_in :
//                          struct in_addr {
//                           in_addr_t s_addr;
//                          };
//                          struct sockaddr_in {
//                           uint8_t sin_len;
//                           sa_family_t sin_family;
//                           in_port_t sin_port;
//                           struct in_addr sin_addr;
//                           char sin_pad[16];
//                          };
// Use struct sockaddr_in to hold an IPv4 address .
// Has the same size and memory layout as struct sockaddr, but interprets the bits differently to give structure to the address .
// 4-4-3--struct sockaddr_in6 :
// struct in6_addr {
//                           uint8_t s6_addr[16];
//                          };
//                          struct sockaddr_in6 {
//                           uint8_t sin6_len;
//                           sa_family_t sin6_family;
//                           in_port_t sin6_port;
//                           uint32_t sin6_flowinfo;
//                           struct in6_addr sin6_addr;
//                          };
// Use struct sockaddr_in6 to hold an IPv6 address .
// Has the same size and memory layout as struct sockaddr, but interprets the bits differently to give structure to the address .


// Active Socket (IRC Client):
// An active socket in IRC represents the client-side connection to an IRC server. The IRC client initiates communication by connecting to the IRC server using a TCP/IP connection. Once connected,
// the client can send commands and messages to the server and receive responses. The IRC client socket handles user input, sends messages to the server, and processes server responses.

// Passive Socket (IRC Server):
// A passive socket in IRC represents the server-side listening socket that accepts incoming connections from IRC clients. The IRC server listens for incoming connections on a specific port,
// When a connection request is received from an IRC client, the server socket accepts the connection, creating a new socket for communication with that client.
// The IRC server socket manages multiple client connections simultaneously, handling commands and messages from each connected client and broadcasting messages to all clients as needed.













// $$ the flow chart of the code and the logic behind the ft-irc project .

//------- Main Points ---------
// 1- Create all the necessary classes and methods for the project.
// 2- Create a socket and handle the server signals.
// 3- Use the poll() function to check if an event has occurred.
// 4- If the event is a new client, register it.
// 5- If the event is data from a registered client, handle it.
// 6- handling the data received from the client .
// 7- handle the basic commands that need to be handled 
//  ---------> (PASS, NICK, USER, JOIN, PART, TOPIC, INVITE, KICK, QUIT, MODE, and PRIVMSG) .
// 8- handle channels .
// 9- check for errors and maintenance the code .


// Resources :
// the flowchart in a detailed way : https://miro.com/app/board/uXjVN086Z8U=/ 
// a detailed IRC overview : https://medium.com/@afatir.ahmedfatir/small-irc-server-ft-irc-42-network-7cee848de6f9 


// Notes :
// I skip some of the informations ,  i only explains new one that i face in this project ,
// topics that i skip :
// OSI Model .
// TCP vs UDP .
// Ports basic informations 

