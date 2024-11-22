#include <server.hpp>
#include <Client.hpp>
#include <Channel.hpp>


/////////////////////////////////////////////
            // ZAKARIA PART
/////////////////////////////////////////////

/////////////////////////////////////////////
            // PASS :
/////////////////////////////////////////////

// Mock definitions for error messages
#define ERR_NEEDMOREPARAMS(nickname, command) "461 " + nickname + " :Not enough parameters for " + command
#define ERR_PASSWDMISMATCH(nickname) "464 " + nickname + " :Password mismatch"
#define FAILURE 0
#define SUCCESS 1

// Mock cmd_struct for command information
struct cmd_struct {
    std::string name;
    std::string message;
};

Client& retrieveClient(Server* server, int client_fd) {
    static Client client("TestUser"); // Static to persist across calls for simplicity
    return client;
}

void addToClientBuffer(Server* server, int client_fd, const std::string& message) {
    std::cout << "Server response to client " << client_fd << ": " << message << std::endl;
}

// Function to extract password from message
std::string retrievePassword(std::string msg_to_parse) {
    std::string password;
    size_t i = 0;

    while (msg_to_parse[i] && msg_to_parse[i] == ' ')
        i++;
    while (msg_to_parse[i] && msg_to_parse[i] != ' ')
        password += msg_to_parse[i++];

    return password;
}

// Function to process the PASS command
int pass(Server *server, int const client_fd, cmd_struct cmd_infos) {
    Client& client = retrieveClient(server, client_fd);
    std::string password = retrievePassword(cmd_infos.message);

    if (cmd_infos.message.empty() || password.empty()) {
        addToClientBuffer(server, client_fd, ERR_NEEDMOREPARAMS(client.getNickname(), cmd_infos.name));
        return FAILURE;
    } else if (server->getPassword() != password) {
        addToClientBuffer(server, client_fd, ERR_PASSWDMISMATCH(client.getNickname()));
        if (!client.isRegistrationDone()) {
            client.setNbInfo(-1);
        }
        return FAILURE;
    } else {
        client.setNbInfo(+1);
        return SUCCESS;
    }
}

/////////////////////////////////////////////
            // NICK :
/////////////////////////////////////////////

// Error and reply message definitions
#define ERR_NONICKNAMEGIVEN(nick) "431 " + nick + " :No nickname given"
#define ERR_ERRONEUSNICKNAME(nick, attempted) "432 " + nick + " :Erroneous nickname " + attempted
#define ERR_NICKNAMEINUSE(nick, attempted) "433 " + nick + " :Nickname is already in use " + attempted
#define RPL_NICK(old_nick, user, new_nick) ":" + old_nick + "!" + user + "@localhost NICK " + new_nick

// Retrieve client by file descriptor
Client& retrieveClient(Server* server, int client_fd) {
    return server->getClients()[client_fd];
}

// Simulate adding a response to the client buffer
void addToClientBuffer(Server* server, int client_fd, const std::string& message) {
    std::cout << "Server response to client " << client_fd << ": " << message << std::endl;
}

// Check if a nickname has invalid characters
bool containsInvalidCharacters(const std::string& nickname) {
    if (nickname.empty() || nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '#')
        return true;

    for (char c : nickname) {
        if (isspace(c) || c == ',' || c == '*' || c == '?' || c == '!' || c == '@' || c == '.')
            return true;
    }
    return false;
}

// Check if a nickname is already used by another client
bool isAlreadyUsed(Server* server, int client_fd, const std::string& new_nickname) {
    const auto& clients = server->getClients();
    for (const std::pair<const int, Client>& entry : clients) {
        const Client& client = entry.second;
        if (entry.first != client_fd && client.getNickname() == new_nickname) {
            return true;
        }
    }
    return false;
}

// Handle the NICK command for nickname change
void nick(Server *server, int client_fd, const std::string& new_nickname) {
    Client& client = retrieveClient(server, client_fd);

    if (new_nickname.empty()) {
        addToClientBuffer(server, client_fd, ERR_NONICKNAMEGIVEN(client.getNickname()));
        return;
    }

    if (containsInvalidCharacters(new_nickname)) {
        addToClientBuffer(server, client_fd, ERR_ERRONEUSNICKNAME(client.getNickname(), new_nickname));
        return;
    }

    if (isAlreadyUsed(server, client_fd, new_nickname)) {
        addToClientBuffer(server, client_fd, ERR_NICKNAMEINUSE(client.getNickname(), new_nickname));
        return;
    }

    client.setNickname(new_nickname);
    addToClientBuffer(server, client_fd, RPL_NICK(client.getOldNickname(), client.getUsername(), client.getNickname()));
}

/////////////////////////////////////////////
            // USER :
/////////////////////////////////////////////

// Mock definitions for error and reply messages
#define ERR_NEEDMOREPARAMS(nick, cmd) "461 " + nick + " " + cmd + " :Not enough parameters"
#define ERR_ALREADYREGISTERED(nick) "462 " + nick + " :You may not reregister"

// Mock classes and functions

Client& retrieveClient(Server* server, int client_fd) {
    return server->getClients()[client_fd];
}

void addToClientBuffer(Server* server, int client_fd, const std::string& message) {
    std::cout << "Server response to client " << client_fd << ": " << message << std::endl;
}

// Command handler for USER
void user(Server *server, int const client_fd, cmd_struct cmd_infos) {
    Client& client = retrieveClient(server, client_fd);
    std::string username = findUsername(cmd_infos.message);
    std::string realname = findRealname(cmd_infos.message);

    if (username.empty() || realname.empty()) {
        addToClientBuffer(server, client_fd, ERR_NEEDMOREPARAMS(client.getNickname(), cmd_infos.name));
    } else if (client.isRegistrationDone()) {
        addToClientBuffer(server, client_fd, ERR_ALREADYREGISTERED(client.getNickname()));
    } else {
        client.setUsername(username);
        client.setRealname(realname);
        client.setNbInfo(+1);
    }
}

// Extracts the username from the message
std::string findUsername(const std::string& msg) {
    std::stringstream ss(msg);
    std::string username;

    // Skip command keyword and extract the username
    std::string temp;
    if (ss >> temp >> username) {
        // Username should be non-empty and skip if it's one of the placeholders (0 *)
        if (username == "0" || username == "*") {
            username.clear();
        }
    }
    return username;
}

// Extracts the realname from the message
std::string findRealname(const std::string& msg) {
    std::stringstream ss(msg);
    std::string realname, temp;

    // Skip to the fourth parameter
    int paramCount = 0;
    while (ss >> temp && paramCount < 3) {
        paramCount++;
    }
    // Everything after the 3rd space is considered the realname
    std::getline(ss, realname);
    if (!realname.empty() && realname[0] == ':') {
        realname.erase(0, 1);  // Remove leading colon if present
    }
    return realname;
}

/////////////////////////////////////////////
            // JOIN:
/////////////////////////////////////////////

Server::Server() : Port(0), Password(""), Serverfd(-1), flags_status(0) {}

Server::Server(const Server& srv) {
    *this = srv;
}

Server& Server::operator=(const Server& src) {
    if (this != &src) {
        Port = src.Port;
        Password = src.Password;
        Serverfd = src.Serverfd;
        flags_status = src.flags_status;
        Clients = src.Clients;
        Channels = src.Channels;
        fds = src.fds;
        msg = src.msg;
    }
    return *this;
}

Server::~Server() {
    Close_filedescriptors();
}

//split
std::vector<std::string> Server::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void Server::handleJoin(Client& client, const std::string& command) {
    // Split command into parts
    std::vector<std::string> params = split(command, ' ');
    if (params.size() < 2) {
        client.sendMessage("461 " + client.getNick() + " JOIN :Not enough parameters");
        return;
    }

    std::vector<std::string> channels = split(params[1], ',');
    std::vector<std::string> keys;
    if (params.size() > 2) {
        keys = split(params[2], ',');
    }

    for (size_t i = 0; i < channels.size(); ++i) {
        std::string channelName = channels[i];
        std::string key = (i < keys.size()) ? keys[i] : "";

        // Check for valid channel name
        if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&')) {
            client.sendMessage("476 " + client.getNick() + " " + channelName + " :Bad Channel Mask");
            continue;
        }

        Channel* channel = getChannel(channelName);
        if (!channel) {
            // Create a new channel
            channel = new Channel(channelName, key);
            Channels.push_back(*channel);
        }

        // Validation checks
        if (channel->isInviteOnly() && !channel->isInvited(client)) {
            client.sendMessage("473 " + client.getNick() + " " + channelName + " :Cannot join channel (+i)");
            continue;
        }
        if (channel->hasKey() && channel->getKey() != key) {
            client.sendMessage("475 " + client.getNick() + " " + channelName + " :Cannot join channel (+k)");
            continue;
        }
        if (channel->isFull()) {
            client.sendMessage("471 " + client.getNick() + " " + channelName + " :Cannot join channel (+l)");
            continue;
        }

        // Add client and notify others
        channel->addMember(client);
        broadcastToChannel(channelName, ":" + client.getNick() + " JOIN " + channelName);

        // Send topic and members list
        if (channel->hasTopic()) {
            client.sendMessage("332 " + client.getNick() + " " + channelName + " :" + channel->getTopic());
        } else {
            client.sendMessage("331 " + client.getNick() + " " + channelName + " :No topic is set");
        }
        client.sendMessage("353 " + client.getNick() + " = " + channelName + " :" + channel->getMembersList());
        client.sendMessage("366 " + client.getNick() + " " + channelName + " :End of /NAMES list");
    }
}

// void Server::Pass_func(int fd, std::string cmd)
// {
//     int position;
//     std::string pass;

//     Client *clio =  getClient(fd);

//     position = cmd.find_first_not_of("\t\v ");
//     if (cmd.empty())
//     {
//         // send a response ----> ERR_NOTENOUGHPARAM;
//     }
//     else if (!clio->getregistred())
//     {
//         pass = cmd;
//         if (pass == Password)
//         {
//             clio->setregistred(true);
//         }
//         // else
//         // send a response ---->  ERR_INCORPASS;
//     }
//     // else
//     // send response ---->  ERR_ALREADYREGISTERED;
// }

// bool Server::Valid_nick_name(std::string& nickname)
// {
//     int i = 0;
//     if (!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
//         return false;
//     while(i < nickname.size() )
//     {
//         if (!std::isalnum(nickname[i]) && nickname[i] != '_')
//             return false;
//     }
//     return true;
// }

// bool Server::Nick_name_already_used(std::string &nickname)
// {
//     int i = 0;
//     while(i < this->Clients.size())
//     {
//         if (this->Clients[i].getnickname() == nickname)
//             return true;
//         return false;
//     }
// }


// void Server::Nick_func(int fd, std::string cmd)
// {
//     int position;
//     std::string granpa;
//     std::string used;
//     Client *clio = getClient(fd);
//     if (cmd.empty())
//     {
//         // send a response ----> ERR_NOTENOUGHPARAM;
//     }
//     if (Nick_name_already_used(cmd) && clio->getnickname() != cmd)
//     {
//         used = "212";
//         if(clio->getnickname().empty())
//             clio->setnickname(used);
//         // send response -----> ERR_NICKINUSE;
//             return;
//     }
//     if (!Valid_nick_name(cmd))
//     {
//         // send response -----> ERR_ERRONEUSNICK;
//         return;
//     }
//     else
//     {
//         if (clio && clio->getregistred())
//         {
//             granpa = clio->getnickname();
//             clio->setnickname(cmd);
//             if(!granpa.empty() && granpa != cmd)
//             {
//                 if(granpa == "212" && !clio->getusername().empty())
//                 {
//                     clio->setlogedstatus(true);
//                     // send response -----> RPL_CONNECTED;
//                     // send response -----> RPL_NICKCHANGE;
//                 }
//                 else
//                     // send response -----> RPL_NICKCHANGE;
//                 return;
//             }
//         }
//         // else if (clio && !clio->getregistred())
//             // send response -----> ERR_NOTREGISTERED;
//     }
//     if (clio && clio->getregistred() && !clio->getusername().empty() && !clio->getnickname().empty() && clio->getnickname() != "212" && !clio->getlogedstatus())
//     {
//         clio->setlogedstatus(true);
//         // send response -----> RPL_CONNECTED;
//     }
// }

// void Server::User_func(int fd, std::string cmd)
// {
//     Client *clio = getClient();
//     // if (this line is used to maintaine the flag condition )
//     if (!clio || !clio->getregistred())
//         // send response -------> ERR_NOTREGISTERED;
//     else if (clio && !clio->getusername().empty())
//         // send response -------> ERR_ALREADYREGISTERED;
//     else
//         clio->setusername(cmd)
//         if (clio && clio->getregistred() && !clio->getusername().empty() && !clio->getnickname().empty() && clio->getnickname() != "212" && !clio->getlogedstatus())
//         clio->setlogedstatus(true)l
//         // send response ------>  RPL_CONNECTED;

// }











/////////////////////////////////////////////
            // OTHMAN PART
/////////////////////////////////////////////

 void Server::executing_commands(int fd, std::string Cmd)
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
       if (Cmd.compare("JOIN") == 0 || Cmd.compare("join") == 0)
             join_func();
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