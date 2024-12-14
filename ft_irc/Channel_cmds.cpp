#include "Channel.hpp"

bool Channel::is_Admin(Client *admin){
    if(std::find(admins.begin(), admins.end(), admin) != admins.end())
        return true;
    return false;
}

bool Channel::is_Invited(Client *admin)
{
    if(std::find(invites.begin(), invites.end(), admin) != invites.end())
        return true;
    return false;
}


bool Channel::is_inChannel(Client *admin)
{
    for (std::vector<Client *>::const_iterator it = Clients.begin(); it != Clients.end(); ++it)
    {
        if (*it == admin)
        {
            return true;
        }
    }
    return false;
}


void Channel::send_to_all(std::string message)
{
    for(size_t i = 0; i < Clients.size(); ++i)
    {
        sendMessage(message, Clients[i]->get_clientfd());
    }
}

void Channel::send_to_all_except_me(std::string message, Client *clio)
{
    for(size_t i = 0; i < Clients.size(); ++i)
    {
        if (Clients[i] != clio)
            sendMessage(message, Clients[i]->get_clientfd());
    }
}


static int keys(const std::string  &str)
{
    for(std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!isdigit(*it))
        {
            return 0;
        }
    }

    std::istringstream ss(str);
    int result;
    ss >> result;

    if (result <= 0)
    {
        return 0;
    }
    return result;
}

void Channel::change_MaxUser(Client *admin, int i, std::string &param, std::string serverIPadd)
{
    std::string reply_message;
    size_t max_us;
    if(i)
    {
        if(param.empty())
        {
            reply_message = ":" +  serverIPadd + ERR_NEEDMOREPARAM(admin->getnickname(), " MODE ");
            sendMessage(reply_message, admin->get_clientfd());
            return;
        }
        max_us= keys(param);
        if(max_us == 0)
        {
            reply_message = ":" + serverIPadd + ERR_INVALIDMODEPARAM(admin->getnickname(), this->GetName() , " l " , param , " Invalid Number . ");
            sendMessage(reply_message, admin->get_clientfd());
        }

        else
        {
            this->has_limit = true;
            SetMaxUsers(max_us);
            SetMode('l');
            reply_message = ":" + admin->getPrefix() + " MODE " + this->GetName() + " +l " + param + "\r\n";
            sendMessage(reply_message, admin->get_clientfd());
        }
    }
    else
    {
        has_limit = false;
        reply_message = ":" + admin->getPrefix() + " MODE " + this->GetName() +  " -l " +  "\r\n";
        sendMessage(reply_message, admin->get_clientfd()); 
    }
}

void Channel::changeInviteMode(Client *admin, bool i)
{
    std::string reply_message;
    if(i)
    {
        this->invite_only = true;
        reply_message = ":" + admin->getPrefix() + " MODE " + this->name + " +i " + "\r\n";
        SetMode('i');    
    }
    else
    {
        this->invite_only = false;
        reply_message = ":" + admin->getPrefix() + " MODE " + this->name + " -i " + "\r\n";
    }
    send_to_all(reply_message);
}

void Channel::changeKeyMode(Client *admin, std::string key, bool i, std::string serverIPadd)
{
    std::string reply_message;
    if(i)
    {
        if(key.empty())
        {
            reply_message = ":" + serverIPadd + ERR_NEEDMOREPARAM(admin->getnickname(), " MODE ");
             return (sendMessage(reply_message, admin->get_clientfd()));
            
        }
        SetPassword(key);
        SetMode('k');
        this->has_password = true;
        reply_message = ":" + admin->getPrefix() + " MODE " + this->GetName() + " +k " + key + "\r\n";
        sendMessage(reply_message, admin->get_clientfd());
    }
    else{
        this->has_password = false;
        reply_message = ":" + admin->getPrefix() + " MODE " + this->GetName() + " -k " + key + "\r\n";
        sendMessage(reply_message, admin->get_clientfd());
    }

}

void Channel::changeTopicMode(Client *admin, bool i)
{
    std::string reply_message;
    if(i)
    {
        this->has_topic = true;
        SetMode('t');
        reply_message = ":" + admin->getPrefix() +  " MODE " + this->GetName() + " +t " + "\r\n";
        send_to_all(reply_message);
    }
    else
    {
        // this->has_rest = false;
        this->has_topic = false;
        reply_message = ":" + admin->getPrefix() +  " MODE " + this->GetName() + " -t " + "\r\n";
        send_to_all(reply_message);
    }
}

void Channel::remove_Invited(Client *admin)
{
    std::vector<Client*>::iterator it = std::find(invites.begin(), invites.end(), admin);
    if (it != invites.end()) {
        invites.erase(it);
    }
}

void Channel::remove_admin(Client *admin)
{
    std::vector<Client*>::iterator it = std::find(admins.begin(), admins.end(), admin);
    if (it != admins.end()) {
        admins.erase(it);
    }
}

void Channel::remove_admino(Client *admin, std::string name, std::string serverIPadd)
{
    std::string  reply_message;
    if (name.empty())
    {
        reply_message = ":" + serverIPadd + ERR_NEEDMOREPARAM(admin->getnickname(), " MODE " + " +o " + name);
        sendMessage(reply_message, admin->get_clientfd());
        return;
    }

    Client *clio = getOperator(name);
    if (clio)
    {
        remove_admin(clio);
        reply_message = ":" + admin->getPrefix() + " MODE " + this->GetName() + " -o " + name + "\r\n";
        send_to_all(reply_message);
    }
    else 
    {
        reply_message = ":" + serverIPadd + ERR_USERNOTINCHANNEL(admin->getnickname(), name , this->GetName());
        sendMessage(reply_message, admin->get_clientfd());
    }

}

void Channel::remove_user(Client *admin)
{
    std::vector<Client*>::iterator it = std::find(Clients.begin(), Clients.end(), admin);
    if (it != Clients.end())
    {
        Clients.erase(it);
    }
}

void Channel::add_admin(Client *admin, std::string name, std::string serverIPadd)
{
    this->operate = true;
    std::string reply_message;
    if(name.empty())
    {
        reply_message = ":" + serverIPadd + ERR_NEEDMOREPARAM(admin->getnickname(), "MODE" + " +o " + name);
        sendMessage(reply_message, admin->get_clientfd());
        return;
    }
    Client *user = GetUser(name);
    if(user)
    {
        admins.push_back(user);
        reply_message = ":" + admin->getPrefix() + " MODE " + this->GetName() + " +o " + name + "\r\n";
        send_to_all(reply_message);
    }
    else
    {
        reply_message = ":" +  serverIPadd + ERR_USERNOTINCHANNEL(admin->getnickname(), name, this->GetName());
        sendMessage(reply_message, admin->get_clientfd());
    }
        
}

void Channel::addUser(Client* client, std::string pass, __unused std::string serverIP)
{
    if (has_password && !validPassio(pass))
    {
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_BADCHANNELKEY(client->getnickname(), pass);
        sendMessage(msg_to_reply, client->get_clientfd());
        return;
    }

    if (has_limit && users + 1 > max_users)
    {
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_CHANNELISFULL(client->getnickname(), GetName());
        sendMessage(msg_to_reply, client->get_clientfd());
        return;
    }

    if (invite_only && !is_Invited(client))
    {
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_INVITEONLYCHAN(client->getnickname(), GetName());
        sendMessage(msg_to_reply, client->get_clientfd());
        return;
    }

    // Prevent duplicates
    if (std::find(Clients.begin(), Clients.end(), client) != Clients.end())
    {
        return;
    }

    // Success: Add user
    Clients.push_back(client);
    users++;
    std::string msg_to_reply = ":" + client->getPrefix() + " JOIN " + GetName() + " * :realname\r\n";
    send_to_all(msg_to_reply);
}


bool    Channel::validPassio(std::string passio)
{
    if (passio == password)
        return true;
    return false;
}
void Server::JOIN(Client* client, std::string& line)
{
    std::string cmd , chan_name , pass;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, pass);
    trim(pass);
    std::string serverio = getServerIP();
    if (chan_name[0] == '#') 
    { 
        Channel *chan = getChannel(chan_name); 
        if (chan == NULL)
        {

            chan = create_channel(client, chan_name , pass);
            addChannel(chan);
        } 
        else 
        {
            chan->addUser(client, pass, serverio);
        }
    }
    else 
    {
        if(client)
        {
            std::string msg_to_send = ":" +  client->getIPaddress()  + ERR_NOSUCHCHANNEL(client->getnickname(), chan_name);
            send(client->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}


bool Server::isNicknameInUse(const std::string &nick)
{
    for (size_t i = 0; i < Clients.size(); ++i)
    {
        if (Clients[i]->getnickname() == nick)
        {
            return true;
        }
    }
    return false;
}

void Server::checkRegistration(Client *client)
{
    if (client->getpsdverified() && !client->getnickname().empty() && !client->getusername().empty())
    {
        client->setregistred(true);
        sendWelcome(client->get_clientfd());
    }
}

void Server::PASS(Client *client, std::string &line)
{
    std::string rawline = line;
    std::istringstream ss(rawline);
    std::string cmd, passio;
    ss >> cmd >> passio;

    line = line.substr(4);
    size_t pos = line.find_first_not_of("\t\v ");
    if (pos < line.size())
    {
        line = line.substr(pos);
        if (line[0] == ':')
            line.erase(line.begin());
    }

    if (pos == std::string::npos || line.empty())
    {
        // Missing password
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_NOTENOUGHPARAM(client->getnickname());
        ssendMessage(msg_to_reply, client->get_clientfd());
    }
    else if (!client->getpsdverified())
    {
        if (line == Password)
        {
            client->setpsdverified(true);
        }
        else
        {
            std::string msg_to_reply = ":" + client->getIPaddress() + ERR_INCORPASS(client->getnickname());
            ssendMessage(msg_to_reply, client->get_clientfd());
        }
    } 
    else 
    {
        // Password already provided
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_ALREADYREGISTERED(client->getnickname());
        ssendMessage(msg_to_reply, client->get_clientfd());
    }

}

void Server::NICK(Client *client, std::string &line)
{
    std::istringstream ss(line);
    std::string cmd, nick;
    ss >> cmd >> nick;

    if (nick.empty())
    {
        // No nickname provided
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_NONICKNAMEGIVEN(client->getnickname());
        ssendMessage(msg_to_reply, client->get_clientfd());
    }
    else if (isNicknameInUse(nick))
    {
        // Nickname already in use
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_NICKNAMEINUSE(client->getnickname(), nick);
        ssendMessage(msg_to_reply, client->get_clientfd());
    }
    else if (nick[0] == '#')
    {
        // Invalid nickname
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_ERRONEUSNICKNAME(client->getnickname(), nick);
        ssendMessage(msg_to_reply, client->get_clientfd());
    }
    else
    {
        // Set nickname and notify other clients
        if (!client->getregistred())
            client->setnickname(nick);
        else
        {

            std::string msg_to_reply = ":" + client->getPrefix() + " NICK :" + nick + "\r\n";
            for (size_t i = 0; i < Clients.size(); ++i)
            {
                ssendMessage(msg_to_reply, Clients[i]->get_clientfd());
            }
            client->setnickname(nick);
        }
    }
    checkRegistration(client);
}

void Server::USER(Client *client, std::string &line)
{
    std::istringstream ss(line);
    std::string cmd, username, hostname, servername, realname;
    ss >> cmd >> username >> hostname >> servername;

    // Extract the realname (remainder of the line after the first three tokens)
    size_t realnamePos = line.find(':');
    if (realnamePos != std::string::npos)
    {
        realname = line.substr(realnamePos + 1);
    }

    if (username.empty() || hostname.empty() || servername.empty() || realname.empty())
    {
        // Missing parameters
        std::string reply_message = ":" + client->getIPaddress() + ERR_NEEDMOREPARAM(client->getnickname(), "USER");
        ssendMessage(reply_message, client->get_clientfd());
    } 
    else if (client->getregistred())
    {
        // Already registered
        std::string msg_to_reply = ":" + client->getIPaddress() + ERR_ALREADYREGISTERED(client->getnickname());
        ssendMessage(msg_to_reply, client->get_clientfd());
    } 
    else
    {
        // Set user-related attributes
        client->setusername(username);
        client->sethostname(hostname);
        client->setservername(servername);
        client->setrealname(realname);
    }

    checkRegistration(client);
}

void Server::KICK(Client *admin, std::string &line)
{
    std::istringstream ss(line);
    std::string cmd, chan_name, nick, reason;
    ss >> cmd >> chan_name >> nick;
    std::getline(ss, reason);
    trim(reason);

    Channel *chan = getChannel(chan_name);
    Client *clio = getClientname(nick);

    // Handle invalid channel
    if (!chan)
    {
        std::string msg_to_send = ":" + admin->getIPaddress() + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
        send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        return;
    }

    // Check if admin has privileges to kick
    if (!chan->is_Admin(admin))
    {
        std::string msg_to_send = ":" + admin->getIPaddress() + ERR_CHANOPRIVSNEEDED(admin->getnickname(), chan_name);
        send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        return;
    }

    // Handle client not found or not on the channel
    if (!clio || !chan->onChannel(clio))
    {
        std::string msg_to_send = ":" + admin->getIPaddress() + ERR_NOTONCHANNEL(nick, chan_name);
        send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        return;
    }
    // Notify all clients in the channel about the kick
    std::string msg_to_reply = ":" + admin->getPrefix() + " KICK " + chan->GetName() + " " + clio->getnickname() + " " + reason + "\r\n";
    chan->send_to_all(msg_to_reply);

    if (chan->is_Admin(clio)) {
        chan->remove_admin(clio); 
    }
    if (chan->is_Invited(clio)) {
        chan->remove_Invited(clio);
    }
    chan->remove_user(clio);
}

void Server::INVITE(Client *admin, std::string &line)
{
    std::string reply_message;
    std::string cmd, nick , chan_name;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> nick;
    std::getline(ss, chan_name);
    trim(chan_name);

    Client *clio = getClientname(nick);
    Channel *chan = getChannel(chan_name);

    if (clio)
    {
        if(chan)
        {
            if(chan->is_Admin(admin))
            {
                if (!chan->is_inChannel(clio))
                {
                    chan->invites.push_back(clio);
                    std::string msg_to_reply =  ":" + admin->getIPaddress() + RPL_INVITING(admin->getnickname() , clio->getnickname() , chan_name);
                    ssendMessage(msg_to_reply, admin->get_clientfd());

                    msg_to_reply = ":" + admin->getPrefix() + " INVITE " + clio->getnickname() + " " + chan->GetName() + "\r\n" ;
                    ssendMessage(msg_to_reply, clio->get_clientfd());
                }
                else
                {
                    std::string msg_to_reply = ":" + admin->getIPaddress() + ERR_USERONCHANNEL(admin->getnickname() , clio->getnickname(), chan_name);
                    ssendMessage(msg_to_reply, admin->get_clientfd());
                }
            }
            else
            {
                std::string msg_to_reply = ":" + admin->getIPaddress() + ERR_CHANOPRIVSNEEDED(admin->getnickname(), clio->getnickname());
            }
        }
        else 
        {
                std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
    else
    {
        if (admin)
        {
            std::string msg_to_send = ":" + admin->getIPaddress() + ERR_NOSUCHNICK(admin->getnickname(), nick);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}

void Server::TOPIC(Client *admin, std::string &line)
{
    std::istringstream ss(line);
    std::string cmd , chan_name , topic;
    std::string reply_message;
    std::string error_message;

    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, topic);
    trim(topic);
    if (!topic.empty() && topic[0] == ':')
    {
        topic.erase(0, 1); 
    }
    Channel *chan = getChannel(chan_name);

    if (topic.empty())
    {
        if(!chan->get_topic().empty())
        {
                reply_message = ":" +  admin->getIPaddress() + RPL_TOPIC(admin->getnickname(), chan->GetName(), chan->get_topic());
                ssendMessage(reply_message, admin->get_clientfd());

                reply_message = ":" + admin->getIPaddress() + RPL_TOPICWHOTIME(admin->getnickname(), chan->GetName(), admin->getnickname(), std::to_string(chan->get_topictime()));
                ssendMessage(reply_message, admin->get_clientfd());
        }
        else
        {
            reply_message = ":" + admin->getIPaddress() + RPL_NOTOPIC(admin->getnickname(), chan->GetName());
            ssendMessage(reply_message, admin->get_clientfd());
        }
        return;
    }

    if (chan)
    {
        if(chan->is_Admin(admin))
        {
            chan->SetTopic(topic);
            reply_message = ":" + admin->getPrefix() + " TOPIC " + chan->GetName() + " :" + topic + "\r\n";
            chan->send_to_all(reply_message);
            return;
        }

        if(topic.find(':') == std::string::npos)
        {
            if (chan->is_inChannel(admin))
            {
                if(!chan->get_has_topic())
                {
                    chan->SetTopic(topic);
                    reply_message = ":" + admin->getPrefix() + " TOPIC " + chan->GetName() + " " + topic + "\r\n";
                    chan->send_to_all(reply_message);
                    return;
                }
                else
                {
                    reply_message = ":" + admin->getIPaddress() + ERR_CHANOPRIVSNEEDED(admin->getnickname(), chan->GetName());
                    ssendMessage(reply_message, admin->get_clientfd());
                }
            }
            else 
            {
                std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOTONCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }
            return;
        }
    }
    else
    {
        if (admin)
        {
             std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}

void Server::PART(Client *admin, std::string &line)
{
    std::string cmd , chan_name , reason;

    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, reason);
    trim(reason);
    Channel *chan = getChannel(chan_name);
    std::string reply_message;

    if (chan)
    {
        if (chan->is_inChannel(admin))
        {

            reply_message = ":" + admin->getPrefix() + " PART " + chan->GetName() + " :leaving channel\r\n";
            chan->send_to_all(reply_message);

            chan->remove_admin(admin);
            chan->remove_user(admin);
            chan->remove_Invited(admin);
        }
        else
        {
            std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOTONCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }

    }
    else
    {
        if (admin)
        {
            std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}

bool	Channel::onChannel(Client *admin) 
{
	for (size_t i = 0; i < Clients.size(); ++i)
    {
		if (admin == Clients[i])
			return true;
	}
	return false;
}

void Server::PRIVMSG(Client *admin, std::string &line)
{   
    std::string cmd , nicknameiichannel , msg;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> nicknameiichannel;
    std::getline(ss, msg);
    trim(msg);
    if (nicknameiichannel[0] == '#')
    {
        Channel *chan = getChannel(nicknameiichannel);
        if (chan)
        {
            if (!chan->onChannel(admin))
            {
                std::string error_msg = ":" + admin->getIPaddress() + ERR_NOTONCHANNEL(admin->getnickname(), chan->GetName());
                send(admin->get_clientfd(), error_msg.c_str(), error_msg.length(), 0);
            }
            size_t pos = msg.find(':');
            if (pos != std::string::npos)
            {
                msg = msg.substr(pos + 1);
            }
            std::string msg_to_send = ":" + admin->getPrefix() + " PRIVMSG " + chan->GetName() + " :" + msg + "\r\n";
            chan->send_to_all_except_me(msg_to_send, admin);
        }
        else 
        {
            if (admin)
            {
                std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), nicknameiichannel);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }
        }
    }
    else 
    {
        Client *clio = getClientname(nicknameiichannel);
        if (clio)
        {
            size_t pos = msg.find(':');
            if (pos != std::string::npos)
            {
                msg = msg.substr(pos + 1);
            }
            std::string msg_to_send = ":" + admin->getPrefix() + " PRIVMSG "  + clio->getnickname() + " :" + msg  + "\r\n";
            ssendMessage(msg_to_send , clio->get_clientfd());
        }
        else 
        {
            if (admin)
            {
                std::string msg_to_send = ":" + admin->getIPaddress() + ERR_NOSUCHNICK(admin->getnickname(), nicknameiichannel);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }
        }
    } 
}

void Server::WHO(Client* admin, std::string &line)
{
    std::string chan_name , cmd;

    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;

    std::string reply_message;
    Channel *chan = getChannel(chan_name);

    if (!chan)
    {
        if(admin)
        {
                std::string msg_to_send = ":" +  admin->getIPaddress()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}
void Server::removeChannel(Channel *chan)
{
    std::string msg_to_reply;

    std::vector<Channel *>::iterator it = std::find(Channels.begin() , Channels.end(), chan);
    if (it != Channels.end())
    {
        Channels.erase(it);
    }
    delete chan;
}

void Server::QUIT(Client *clio, std::string &line)
{
    std::string cmd, reason;
    std::istringstream ss(line);

    ss >> cmd;
    std::getline(ss, reason);
    trim(reason);

    if (reason.empty())
        reason = "Quit";
    
    if (reason == ":leaving")
    {
        
    } 

    for (size_t i = 0; i < Channels.size(); ++i)
    {
        if (Channels[i]->GetUser(clio->getnickname()) || Channels[i]->getOperator(clio->getnickname()))
        {
            std::string msg_to_send = ":" + clio->getnickname() + "!~" + clio->getusername() + "@localhost QUIT " + reason + "\r\n";

            Channels[i]->send_to_all(msg_to_send);
            
            Channels[i]->remove_admin(clio);
            Channels[i]->remove_user(clio);
            Channels[i]->remove_Invited(clio);

            if (Channels[i]->GetClientsNumber() == 0)
                removeChannel(Channels[i]);

            close(clio->get_clientfd());

            std::vector<Client*>::iterator it = std::find(Clients.begin(), Clients.end(), clio);
            if (it != Clients.end())
            {
                Clients.erase(it);
            }
            delete clio;
        }
    }
}


void Server::MODE( Client *client, std::string &line)
{
    std::string cmd , chan_name, modes, arg, reply_message;
    std::istringstream ss(line);
    ss >> cmd >> chan_name >> modes >> arg;
    Channel *chan = getChannel(chan_name);
    // std::string serverIPadd = getServerIP();
    if(chan)
    {
        if(!modes.empty())
        {
            if(chan->is_Admin(client))
            {
                chan->valid_mode(client, modes, arg, client->getIPaddress());
            }
            else
            {
                if(client)
                    reply_message = ":" + client->getIPaddress() + ERR_CHANOPRIVSNEEDED(client->getnickname(), chan_name);
            } 
        }
        else
        {
            if(!chan->get_topic().empty())
                chan->rpl_topic(client, "fda",  client->getIPaddress());
            chan->rpl_list(client,  client->getIPaddress());
            chan->rpl_mode(client, client->getIPaddress());
        }
    }
    else
    {
        if(client)
        {
            reply_message = ":" + client->getIPaddress() + ERR_NOSUCHCHANNEL(client->getnickname(), chan_name);
            ssendMessage( reply_message, client->get_clientfd());
        }
    }
}


void Server::Bot_call(Client *admin, std::string line)
{
    std::string cmd, chan_name ,Order;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, Order);
    trim(Order);
    std::string reply_message;

    std::vector<std::string> jokes;
    jokes.push_back("Why did the scarecrow win an award? Because he was outstanding in his field!");
    jokes.push_back("What do you get when you cross a snowman and a vampire? Frostbite.");
    jokes.push_back("Why don't scientists trust atoms? Because they make up everything!");
    jokes.push_back("What do you call a fish wearing a crown? A kingfish.");
    jokes.push_back("What do you call a pile of cats? A meowtain.");
    jokes.push_back("What do you call a bear with no teeth? A gummy bear.");
    jokes.push_back("What do you call a cow with two legs? Lean beef.");
    jokes.push_back("Why did the bicycle fall over? Because it was two-tired!");
    jokes.push_back("How does a penguin build its house? Igloos it together!");

    std::vector<std::string> insults;
    insults.push_back("You're a grey sprinkle on a rainbow cupcake.");
    insults.push_back("If you were a vegetable, you'd be a cabbitch.");
    insults.push_back("You are more disappointing than an unsalted pretzel.");
    insults.push_back("You're the human version of period cramps.");
    insults.push_back("You're a pizza burn on the roof of the world's mouth.");
    insults.push_back("You're a mistake of nature.");
    insults.push_back("You're like a cloud. When you disappear, it's a beautiful day.");
    insults.push_back("You have the perfect face for radio.");
    insults.push_back("You're proof that even Google doesn't have all the answers.");

    std::vector<std::string> compliments;
    compliments.push_back("You're doing amazing!");
    compliments.push_back("Keep up the great work!");
    compliments.push_back("You're a wonderful person!");
    compliments.push_back("You make the world a better place!");
    compliments.push_back("You're an inspiration!");

    if (Order == "joke")
    {
        reply_message = jokes[rand() % jokes.size()];
    }
    else if (Order == "insult")
    {
        reply_message = insults[rand() % insults.size()];
    }
    else if (Order == "greet")
    {
        reply_message = "Hello everyone! Hope you're all having a great day!";
        for (size_t i = 0; i < Clients.size(); ++i)
        {
            std::string msg_to_send = ":" + admin->getPrefixBot() + " PRIVMSG " + chan_name + " " + admin->getnickname() + " :" + reply_message + "\r\n";
            ssendMessage(msg_to_send, Clients[i]->get_clientfd());
        }
        return;
    }
    else if (Order == "compliment")
    {
        reply_message = compliments[rand() % compliments.size()];
    }
    else if (Order == "help")
    {
        reply_message = "Available bot commands: 'joke' - 'insult' - 'greet' - 'compliment'";
    }
    else
    {
        reply_message = "I don't understand that command. Please use 'joke', 'insult', 'greet', or 'compliment'.";
    }

    std::string msg_to_send = ":" + admin->getPrefixBot() + " PRIVMSG " + chan_name+ " " + Order + " :" + reply_message + "\r\n";
    ssendMessage(msg_to_send, admin->get_clientfd());
}
