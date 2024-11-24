
#include "Channel.hpp"



bool Channel::is_Admin(Client *admin){
    if(std::find(admins.begin(), admins.end(), admin) != admins.end())
        return true;
    return false;
}

bool Channel::is_inChannel(Client *admin){
    for (std::vector<Client *>::const_iterator it = Clients.begin(); it != Clients.end(); ++it) {
        if (*it == admin) {
            return true;
        }
    }
    return false;
}


void Channel::send_to_all(std::string message){
    for(size_t i = 0; i < Clients.size(); ++i){
        setbuffer(message, Clients[i]->get_clientfd());
    }
}


void Channel::admin_MODE(Client *admin, std::string mode, std::string arg){
    std::string reply_message;
    char mode_char = mode[0];
    size_t i = 0;
    if (mode_char == '+')
        i++;
    if (mode_char != '-'){
        for(; i < mode.size(); ++i){
                if (mode == "i"){
                    this->modes += "i";
                    changeInviteMode(admin, true);
                }
                else if (mode == "k"){
                    this->modes += "k";
                    changeKeyMode(admin, arg, true);
                }
                else if (mode == "t"){
                    this->modes += "t";
                    changeTopicMode(admin, true);
                }
                else if (mode == "o"){
                    this->modes += "o";
                    add_admin(admin, arg);
                }
                else if (mode == "l"){
                    this->modes += "l";
                    change_MaxUser(admin, 1, arg);
                }
                else{
                    reply_message = GetUserInfo(admin, false) + ERR_UNKNOWNMODE(admin->getnickname(), mode[i]);
                    setbuffer(reply_message, admin->get_clientfd());
                }
        }
    }
    else{
        for(; i < mode.size(); ++i){
            if (mode == "i"){
                this->modes += "i";
                changeInviteMode(admin, false);
            }
            else if (mode == "k"){
                this->modes += "k";
                changeKeyMode(admin, arg, false);
            }
            else if (mode == "t"){
                this->modes += "t";
                changeTopicMode(admin, false);
            }
            else if (mode == "o"){
                this->modes += "o";
                remove_admin(admin, arg);
            }
            else if (mode == "l"){
                this->modes += "l";
                change_MaxUser(admin, 0, arg);
            }
            else{
                reply_message = GetUserInfo(admin, false) + ERR_UNKNOWNMODE(admin->getnickname(), mode[i]);
                setbuffer(reply_message, admin->get_clientfd());
            }  
        }
    }
}

void Channel::change_MaxUser(Client *admin, int i, std::string &param){
    std::string reply_message;
    int max_users = std::stoi(param);//change this later
    if(i){
        if(param.empty()){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +l ");
            setbuffer(reply_message, admin->get_clientfd());
            return;
    }
        if(max_users <= 0){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +l ");
            setbuffer(reply_message, admin->get_clientfd());
            return;
        }
        else{
            has_limit = true;
            SetMaxUsers(max_users);
            send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +l "));
        }
    }
    else{
        has_limit = false;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +l "));
    }
}

void Channel::changeInviteMode(Client *admin, bool i){
    std::string reply_message;
    if(i){
        this->invite_only = true;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +i" ));
    }
    else{
        this->invite_only = false;
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -i" ));
    }
}

void Channel::changeKeyMode(Client *admin, std::string key, bool i){
    std::string reply_message;
    if(i){
        if(key.empty()){
            reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +k");
            setbuffer(reply_message, admin->get_clientfd());
            return;
        }
        else{
            SetPassword(key);
            this->has_password = true;
            reply_message = GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +k" );
        }
    }
    else{
        this->has_password = false;
        reply_message = GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -k" );
    }

}

void Channel::changeTopicMode(Client *admin, bool i){
    std::string reply_message;
    if(i){
        this->has_topic = true;
        send_to_all(GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +t" ));
    }
    else{
        this->has_topic = false;
        send_to_all(GetUserInfo(admin, false) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -t" ));
    }
}

void Channel::add_admin(Client *admin, std::string name){
    this->operate = true;
    std::string reply_message;
    if(name.empty()){
        reply_message = GetUserInfo(admin, false) + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +o " + name);
        setbuffer(reply_message, admin->get_clientfd());
        return;
    }
    Client *user = GetUser(name);
    if(user){
        admins.push_back(user);
        send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +o " + admin->getnickname()));
    }
    else{
        reply_message= GetUserInfo(user, false) + ERR_USERNOTINCHANNEL(admin->getnickname(), user->getnickname(), this->GetName());
        setbuffer(reply_message, admin->get_clientfd());
    }
        
}

void Channel::addUser(Client* client)
{
        Clients.push_back(client);  // Adds the client to the 'beta_users' vector
}

void Channel::remove_user(Client *admin) {
    std::vector<Client*>::iterator it = std::find(Clients.begin(), Clients.end(), admin);
    if (it != Clients.end())
        Clients.erase(it);
    std::vector<Client*>::iterator it1 = std::find(admins.begin(), admins.end(), admin);
    if (it1 != admins.end())
        admins.erase(it1);//if anythign goes wrong here, check the iterator
    if(invite_only){
        std::vector<Client*>::iterator it2 = std::find(invites.begin(), invites.end(), admin);
        if (it2 != invites.end())
            invites.erase(it2);//if anythign goes wrong here, check the iterator
    }
}

void Channel::remove_admin(Client *admin, std::string name) {
    this->operate = false;
    std::string reply_message;
    Client *user = GetUser(name);

    if (user) {
        std::vector<Client*>::iterator it = std::find(admins.begin(), admins.end(), admin);

        if (it != admins.end()){
            admins.erase(it);
            send_to_all(GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -o " + admin->getnickname()));
        }
    }
    else {
        reply_message = GetUserInfo(user, false) + ERR_USERNOTINCHANNEL(admin->getnickname(), user->getnickname(), this->GetName());
        setbuffer(reply_message, admin->get_clientfd());
    }
}

void Channel::MODE(Client *admin, std::string mode, std::string arg){
    std::cout << " Ara wa7ed Mode hna --#%& " << std::endl;
    std::string reply_message;
    if(!mode.empty()){
        if(is_Admin(admin))
            admin_MODE(admin, mode, arg);
        else{
            reply_message = GetUserInfo(admin, false) + ERR_CHANOPRIVSNEEDED(admin->getnickname(), this->GetName());
            setbuffer(reply_message, admin->get_clientfd());
            return;
        }
    }
    else{
        reply_message = GetUserInfo(admin, true) + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), this->modes);
        //rpl list of modes
        reply_message += GetUserInfo(admin, false) + " 353 " + admin->getnickname() + " = " + this->GetName() + " :";
        for(size_t i = 0; i < Clients.size(); ++i){
            reply_message += "@" + Clients[i]->getnickname() + " ";
        }
        reply_message += "\r\n";
        setbuffer(reply_message, admin->get_clientfd());
        setbuffer(GetUserInfo(admin, false) + RPL_ENDOFWHOIS(admin->getnickname(), this->GetName()), admin->get_clientfd());
        setbuffer(reply_message, admin->get_clientfd());
        reply_message = GetUserInfo(admin, true) + RPL_CREATIONTIME(admin->getnickname(), this->GetName(), this->get_time());
        setbuffer(reply_message, admin->get_clientfd());
    }
}

void Channel::KICK(Client *admin, Client *user, std::string reason){
    std::cout << " Ara wa7ed kick hna --#%& " << std::endl;
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->getnickname(), this->GetName());
        setbuffer(reply_message, admin->get_clientfd());
        return;
    }
    //isalpha
    if(!is_inChannel(user)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(user->getnickname(), this->GetName());
        setbuffer(reply_message, admin->get_clientfd());
        return;
    }
    send_to_all(GetUserInfo(user, true) + "KICK " + this->GetName() + " " + user->getnickname() + " :" + (reason.empty() ? "bad content" : reason) + "\r\n");
    remove_user(user);
}


void Channel::INVITE(Client *admin, Client *user){
    std::cout << " Ara wa7ed Invite hna --#%& " << std::endl;
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->getnickname(), this->GetName());
        setbuffer(reply_message, admin->get_clientfd());
        return;
    }
    if(!is_inChannel(user)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(user->getnickname(), this->GetName());
        setbuffer(reply_message, admin->get_clientfd());
        return;
    }
    //isalpha
    this->invites.push_back(user);
    setbuffer(GetUserInfo(admin, false) + RPL_INVITING(admin->getnickname(), user->getnickname(), this->GetName()), user->get_clientfd());
    setbuffer(GetUserInfo(admin, false) + RPL_INVITING(admin->getnickname(), user->getnickname(), this->GetName()), admin->get_clientfd());
}

void Channel::TOPIC(Client *admin, std::string topic_message){
    std::cout << " Ara wa7ed Topic hna --#%& " << std::endl;
    std::string reply_message;
    std::string error_message;
    if(!is_inChannel(admin)){
        error_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->getnickname(), this->GetName());
        setbuffer(error_message, admin->get_clientfd());
        return;
    }
    if(has_topic){
        if(!topic.empty()){
            SetTopic(topic_message);
			reply_message = GetUserInfo(admin, true) + RPL_TOPIC(admin->getnickname(), this->GetName(), topic_message);
            setbuffer(reply_message, admin->get_clientfd());
            return;
        }
        else{
            reply_message = GetUserInfo(admin, true) + RPL_NOTOPIC(admin->getnickname(), this->GetName());
            setbuffer(reply_message, admin->get_clientfd());
            return;
        }
    }
    else{
        if(!topic_message.empty()){
            SetTopic(topic_message);
            reply_message = GetUserInfo(admin, true) + RPL_TOPIC(admin->getnickname(), this->GetName(), topic_message);
            setbuffer(reply_message, admin->get_clientfd());
            return;
        }
        else{
            reply_message = GetUserInfo(admin, false) + RPL_NOTOPIC(admin->getnickname(), this->GetName());
            setbuffer(reply_message, admin->get_clientfd());
            return;
        }
    }
}

void Channel::PART(Client *admin, std::string reason){
    std::cout << " Ara wa7ed Part hna --#%& " << std::endl;
    std::string reply_message;
    if(!is_inChannel(admin)){
        reply_message = GetUserInfo(admin, false) + ERR_NOTONCHANNEL(admin->getnickname(), this->GetName());
        setbuffer(reply_message, admin->get_clientfd());
    }
    else{
        remove_user(admin);
		send_to_all(GetUserInfo(admin, true) + " PART " + this->GetName() + " " + reason + "\r\n");
    }
}
// Special Considerations: !!!!!!!!!!!
// If the last operator leaves a channel (e.g., by using PART), the channel may be left without any operator unless another user is automatically promoted to operator. In some IRC implementations, this happens to ensure channel continuity.
// An operator leaving via PART does not override channel restrictions like invite-only mode (+i) or password protection (+k). Rejoining the channel would still require them to adhere to those rules.
/////////////////////////////////////


bool Channel::onChannel(Client *admin){
    if(std::find(Clients.begin(), Clients.end(), admin) != Clients.end())
        return true;
    return false;
}

int Channel::PRIVMSG(Client *admin, Client *target, std::string message) {
    std::cout << " Ara wa7ed Privmsg hna --#%& " << std::endl;
    if (message.find("#") == 0){
        if (!this->onChannel(admin)){
            std::string error_msg = ERR_NOTONCHANNEL(admin->getnickname(), this->GetName());
            send(admin->get_clientfd(), error_msg.c_str(), error_msg.length(), 0);
            return -1;
        }
        for (size_t i = 0; i < Clients.size(); ++i) {
            if (Clients[i]->getnickname() == admin->getnickname()) {
                continue;
            }
            std::string msg_to_send = ":" + admin->getnickname() + "!" + admin->getusername() + "@localhost PRIVMSG " + this->GetName() + " :" + message + "\r\n";
            send(Clients[i]->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
        return 0;
    }

    if (target == NULL || target->getnickname().empty()){
        std::string error_msg = ERR_NOSUCHNICK(admin->getnickname(), "unknown");
        send(admin->get_clientfd(), error_msg.c_str(), error_msg.length(), 0);
        return -1;
    }

    std::string msg_to_send = ":" + admin->getnickname() + "!" + admin->getusername() + "@localhost PRIVMSG " + target->getnickname() + " :" + message + "\r\n";
    send(target->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
    return 0;
}

// void WHO(Client *admin, Client* target)
// {
//     // check if the arg is channel , or a specific user 
//         // if it a user 
//             // search for him in Clients vector in channel class 
//                 // print his public data

//         // if it is a channel
//             // search for it in Channels in server class 
//                 // print all their members 
// }

// void Channel::WHO(Client *admin, const std::string &target, Server *srv)
// {
//     // Check if the target is a channel
//     if (target[0] == '#') { // It's a channel
//         // Search for the channel in the server's channel list using GetChannel
//         Channel* chan = srv->GetChannel(target);
//         if (chan != NULL) {
//             // Channel found, check the members
//             std::vector<Client*> members = chan->getMembers(); // Assuming Channel class has getMembers()
//             for (size_t i = 0; i < members.size(); ++i) {
//                 Client* member = members[i];

//                 // Check if the member is visible:
//                 // 1. The user is visible if they aren't invisible (+i mode)
//                 // 2. Or the requesting client and the user share at least one channel

//                 // Check if the member is invisible
//                 bool isInvisible = member->hasMode("i");  // Assuming `hasMode` checks if the user has the specified mode
//                 bool hasCommonChannel = false;

//                 // Check if the admin (requesting client) and the member share a channel
//                 std::vector<Channel*> adminChannels = admin->getChannels(); // Assuming `getChannels` returns the channels the client is in
//                 for (size_t j = 0; j < adminChannels.size(); ++j) {
//                     for (size_t k = 0; k < member->getChannels().size(); ++k) {
//                         if (adminChannels[j] == member->getChannels()[k]) {
//                             hasCommonChannel = true;
//                             break;
//                         }
//                     }
//                     if (hasCommonChannel) break;
//                 }

//                 // If the member is visible (not invisible or has a common channel with the admin)
//                 if (!isInvisible || hasCommonChannel) {
//                     // Send member's public data to the admin
//                     admin->sendMessage("User: " + member->getnickname() + " (" + member->getusername() + ")");
//                 }
//             }
//         } else {
//             // Channel not found
//             admin->sendError("403", target, "No such channel");
//         }
//     } else { // It's a user
//         // Search for the user in the server's client list using GetClient (you need to implement GetClient)
//         Client* client = srv->GetClient(target);
//         if (client != NULL) {
//             // Check if the client is visible (not invisible or has a common channel with the admin)
//             bool isInvisible = client->hasMode("i");  // Assuming `hasMode` checks if the user has the specified mode
//             bool hasCommonChannel = false;

//             // Check if the admin (requesting client) and the user share a channel
//             std::vector<Channel*> adminChannels = admin->getChannels();
//             for (size_t j = 0; j < adminChannels.size(); ++j) {
//                 for (size_t k = 0; k < client->getChannels().size(); ++k) {
//                     if (adminChannels[j] == client->getChannels()[k]) {
//                         hasCommonChannel = true;
//                         break;
//                     }
//                 }
//                 if (hasCommonChannel) break;
//             }

//             // If the client is visible (not invisible or has a common channel with the admin)
//             if (!isInvisible || hasCommonChannel) {
//                 // Send public data
//                 admin->sendMessage("Nickname: " + client->getnickname() + 
//                                     "\nUsername: " + client->getusername() + 
//                                     "\nRealname: " + client->getrealname());
//             }
//         } else {
//             // User not found
//             admin->sendError("401", target, "No such user");
//         }
//     }
// }

