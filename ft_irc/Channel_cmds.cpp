
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

/////////////////////////////////////

// void Channel::NICK(Client *admin, const std::string new_nick) {//reference--------

//     for (size_t i = 0; i < Clients.size(); ++i) {
//         if (Clients[i]->getnickname() == new_nick) {
//             std::string message = GetUserInfo(admin, false) + " " + admin->getnickname() + " " + new_nick + ":getnickname() is already in use\r\n";
//             send(admin->get_clientfd(), message.c_str(), message.size(), 0);
//             return;
//         }
//     }

//     admin->getnickname() = new_nick;
// }

// void Channel::USER(Client *admin, std::string username, std::string realname) {
//     if(realname.find(":")){
//         std::string message = ERR_NEEDMOREPARAMS1();
//         send(admin->get_clientfd(), message.c_str(), message.size(), 0);
//     }
//     else{
//         admin->username = username;
//         admin->realname = realname;
//     }
    
// }

// void Channel::PASS(Client *admin, std::string password) {
//     // if(password == this->server.password)//ta nmergiw w assigni hada l password d server
//         admin->has_password = true;
//     // else{
//         std::string message = ERR_PASSWDMISMATCH();
//         send(admin->get_clientfd(), message.c_str(), message.size(), 0);
//     // }
// }

int Channel::PRIVMSG(Client *admin, Client *target, std::string message) {
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