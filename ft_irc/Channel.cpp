#include "Channel.hpp"

Channel::Channel()
{
    name = "default";
    invite_only = false;
    has_password = false;
    has_topic = true;
    has_limit = false;
    operate = false;
    max_users = 0;
    creation_time = std::time(NULL);
}

Channel::Channel(const std::string& name) : name(name) 
{
    invite_only = false;
    has_password = false;
    has_topic = true;
    has_limit = false;
    operate = false;
    max_users = 0;
    users = 0;
    mode = "";
    creation_time = std::time(NULL);
}

Channel::Channel(const std::string& name, const std::string& pswd) : name(name), password(pswd) 
{
    has_password = false;
    invite_only = false;
    has_topic = true;
    has_limit = false;
    operate = false;
    max_users = 0;
    users = 0;
    mode = "";
    creation_time = std::time(NULL);
}


Channel &Channel::operator=(const Channel &src)
{
    if(this == &src)
        return *this;
    name = src.name;
    topic = src.topic;
    mode = src.mode;
    password = src.password;
    invite_only = src.invite_only;
    has_password = src.has_password;
    has_topic = src.has_topic;
    has_limit = src.has_limit;
    operate = src.operate;
    max_users = src.max_users;
    users = src.users;
    Clients = src.Clients;
    admins = src.admins;
    invites = src.invites;
    creation_time = src.creation_time;
    topic_time = src.topic_time;
    return *this;
}

Channel::Channel(Channel const &src)
{
    *this = src;
}

Channel::~Channel()
{
    Clients.clear();
    admins.clear();
    invites.clear();
}

void Channel::SetName(std::string name)
{
    this->name = name;
}

void Channel::SetMode(char mode)
{
    this->mode = mode;
}

void Channel::SetTopic(std::string Topic)
{
    topic_time = std::time(NULL);
    this->topic = Topic;
}

void Channel::SetPassword(std::string passd)
{
	this->password = passd;
}

std::time_t Channel::get_topictime()
{
    return topic_time;
}

std::string size_tToString(size_t value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
std::string Channel::get_modes()
{
    std::string modes = "+";
    
    modes += this->has_topic ? "t" : "";
    modes += this->has_limit ? "l": "";
    modes += this->invite_only ? "i": "";
    modes += this->has_password ? "k": "";
    modes += this->has_limit ? " " + size_tToString(this->max_users): "";
    modes += this->has_password ? " " + this->password: "";
    this->mode = modes;

    return mode;
}

void Channel::SetMaxUsers(int max){
    this->max_users = max;
    has_limit = true;
}

void Channel::SetTime(std::time_t time)
{
    this->creation_time = time;
}

void Channel::set_has_password(bool has_password)
{
    this->has_password = has_password;
}

void Channel::sendMessage(std::string message, int destination_fd)
{
    size_t sent = 0;
    size_t msg_len = message.size();
    char *msg = (char *)message.c_str();
    while(sent < msg_len)
    {
        int i = send(destination_fd, msg + sent, msg_len - sent, 0);
        if(i == -1)
           return ;
        sent += i;
    }
}

std::string Channel::GetName(){
    return name;
}

std::time_t Channel::get_time()
{
	return this->creation_time;
}

bool Channel::get_has_password()
{
    return this->has_password;
}

bool Channel::get_has_topic()
{
    return this->has_topic;
}

bool Channel::get_invite_only()
{
    return this->invite_only;
}

bool Channel::get_limit()
{
    return this->has_limit;
}
size_t Channel::get_maxusers()
{
    return this->max_users;
}

std::string Channel::get_password()
{
    return this->password;
}

std::string Channel::get_topic()
{
    return this->topic;
}
Client *Channel::GetUser(std::string name)
{
    for(size_t i = 0; i < Clients.size(); ++i)
    {
        if(name == Clients[i]->getnickname())
            return Clients[i];
    }
    return NULL;
}

size_t Channel::GetClientsNumber()
{
    // check it later :
    return Clients.size() + admins.size();
}


Client *Channel::getOperator(std::string name)
{
    for(size_t i = 0; i < admins.size(); ++i)
    {
        if(name == admins[i]->getnickname())
            return admins[i];
    }
    return NULL;
}

std::vector<Client *>  Channel::getMembers()
{
    return Clients;
}

std::string Channel::GetUserInfo(Client *admin, bool i)
{
    if(i)
        return ":" + admin->getnickname() + "!" + admin->getusername() + "@" + admin->getservername() + " ";
    else
        return ":" + admin->getservername();
}


void Channel::valid_mode(Client *cli, std::string &modes, std::string param, std::string serverIPadd)
{
    char mode_char = modes[0];
    size_t i = 0;

    if (mode_char == '+')
        i++;
    if (mode_char != '-')
    {
        for(; i < modes.size(); ++i)
        {
            if(modes[i] == 'i' || modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o' || modes[i] == 't')
            {
                plus_modes(cli, modes[i], param, serverIPadd);
            }
            else
            { 
                std::string reply_message = ":" + serverIPadd + ERR_UNKNOWNMODE(cli->getnickname(), modes[i]);
                sendMessage(reply_message, cli->get_clientfd());
            }
        }
    }
    else
    {
            for(i = 1; i < modes.size(); ++i)
            {
                if(modes[i] == 'i' || modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o' || modes[i] == 't')
                    minus_modes(cli, modes[i], param, serverIPadd);
                else
                {
                    std::string reply_message =  ":" + serverIPadd + ERR_UNKNOWNMODE(cli->getnickname(), modes[i]);
                    sendMessage(reply_message, cli->get_clientfd());
                }

            }
    }
}

void Channel::plus_modes(Client *cli, char c, std::string param, std::string serverIPadd)
{
    if(c == 'i')
    {
        changeInviteMode(cli, true);
    }
    else if(c == 'k')
    {
        changeKeyMode(cli, param, true, serverIPadd);
    }
    else if(c == 'l')
    {
        change_MaxUser(cli, 1, param, serverIPadd);
    }
    else if(c == 'o')
    {
        add_admin(cli, param , serverIPadd);
    }
    else if(c == 't')
    {
        changeTopicMode(cli, true);
    }
}

void Channel::minus_modes(Client *cli, char c, std::string param , std::string serverIPadd)
{
    if(c == 'i')
    {
        changeInviteMode(cli, false);
    }
    else if(c == 'k'){
        changeKeyMode(cli, param, false, serverIPadd);
    }
    else if(c == 'l'){
        change_MaxUser(cli, 0, param, serverIPadd);
    }
    else if(c == 'o'){
        remove_admino(cli, param, serverIPadd);
    }
    else if(c == 't'){
        changeTopicMode(cli, false);
    }
    if (mode.find(c) != std::string::npos)
    {
        mode.erase(mode.find(c), 1);
    }
}

void Channel::rpl_topic(Client *cli, std::string topic, std::string serverIPadd)
{
    std::string reply_message;
    if(topic.find(':') == std::string::npos)
    {
        if(onChannel(cli))
        {
            if(get_has_topic())
            {
                reply_message = ":" + serverIPadd + " " + RPL_TOPIC(cli->getnickname(), this->GetName(), topic);
                sendMessage(reply_message, cli->get_clientfd());
                reply_message = ":" + serverIPadd + " " + RPL_TOPICWHOTIME(cli->getnickname(), this->GetName(), cli->getnickname(), std::to_string(get_topictime()));
                sendMessage(reply_message, cli->get_clientfd());
            }
            else{
                reply_message = ":" + serverIPadd + " " + RPL_NOTOPIC(cli->getnickname(), this->GetName());
                sendMessage(reply_message, cli->get_clientfd());
            }
        }
        else
        {
            reply_message = ":" +  serverIPadd + ERR_NOTONCHANNEL(cli->getnickname(), this->GetName());
            sendMessage(reply_message, cli->get_clientfd());
        }
        return;
    }
    if(is_Admin(cli))
    {
        if(mode.find('t') == std::string::npos)
        {
            SetTopic(topic);
			reply_message = ":" + cli->getPrefix() +  " TOPIC " + GetName() + " " + topic + "\r\n";
            send_to_all(reply_message);
        }
        else
        {
			reply_message = ":" + serverIPadd + ERR_INVALIDMODEPARAM(cli->getnickname(), GetName(), " TOPIC ", topic , "Channel Topic Restrection Are On");
            sendMessage(reply_message, cli->get_clientfd());
        }
    }
    else
    {
        reply_message = ":" + serverIPadd + ERR_CHANOPRIVSNEEDED(cli->getnickname(), GetName());
        sendMessage(reply_message, cli->get_clientfd());
    }
}

void Channel::rpl_mode(Client *cli, std::string serverIPadd)
{
    std::string reply_message;
    reply_message = ":" + serverIPadd + RPL_CHANNELMODEIS(cli->getnickname(), GetName(), get_modes());
    sendMessage(reply_message, cli->get_clientfd());
    reply_message = ":" + serverIPadd + RPL_CREATIONTIME(cli->getnickname(), GetName(), std::to_string(get_time()));
    sendMessage(reply_message , cli->get_clientfd());
}

void Channel::rpl_list(Client *cli, std::string serverIPadd)
{
    std::string reply_message;
    reply_message += ":" + serverIPadd + " 353 " + cli->getnickname() + " = ";
    reply_message += GetName() + " :";
    
    for(size_t i = 0; i < Clients.size(); ++i)
    {
        if(is_Admin(Clients[i]))
        reply_message += "@";
        reply_message += Clients[i]->getnickname() + " ";
    }
	reply_message += "\r\n";
    sendMessage(reply_message, cli->get_clientfd());
    reply_message = ":" + serverIPadd + RPL_ENDOFNAMES(cli->getnickname(), GetName());
    sendMessage(reply_message, cli->get_clientfd());
}
