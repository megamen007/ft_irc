
#include "Channel.hpp"



bool Channel::is_Admin(Client *admin){
    if(std::find(admins.begin(), admins.end(), admin) != admins.end())
        return true;
    return false;
}

bool Channel::is_Invited(Client *admin)
{
    if(std::find(invites.begin(), invites.end(), admin) != admins.end())
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


void Channel::admin_MODE(Client *admin, std::string mode, std::string arg){
    std::string reply_message;
    char mode_char = mode[0];
    size_t i = 0;
    if (mode_char == '+')
        i++;
    if (mode_char != '-'){
        for(; i < mode.size(); ++i){
                if (mode == "i"){
                    this->mode += "i";
                    changeInviteMode(admin, true);
                }
                else if (mode == "k"){
                    this->mode += "k";
                    changeKeyMode(admin, arg, true);
                }
                else if (mode == "t"){
                    this->mode += "t";
                    changeTopicMode(admin, true);
                }
                else if (mode == "o"){
                    this->mode += "o";
                    add_admin(admin, arg);
                }
                else if (mode == "l"){
                    this->mode += "l";
                    change_MaxUser(admin, 1, arg);
                }
                else{
                    reply_message = GetUserInfo(admin, false) + ERR_UNKNOWNMODE(admin->getnickname(), mode[i]);
                    sendMessage(reply_message, admin->get_clientfd());
                }
        }
    }
    else{
        for(; i < mode.size(); ++i){
            if (mode == "i"){
                this->mode += "i";
                changeInviteMode(admin, false);
            }
            else if (mode == "k"){
                this->mode += "k";
                changeKeyMode(admin, arg, false);
            }
            else if (mode == "t"){
                this->mode += "t";
                changeTopicMode(admin, false);
            }
            else if (mode == "o"){
                this->mode += "o";
                remove_admin(admin);
            }
            else if (mode == "l"){
                this->mode += "l";
                change_MaxUser(admin, 0, arg);
            }
            else{
                reply_message = GetUserInfo(admin, false) + ERR_UNKNOWNMODE(admin->getnickname(), mode[i]);
                sendMessage(reply_message, admin->get_clientfd());
            }  
        }
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

void Channel::change_MaxUser(Client *admin, int i, std::string &param)
{
    std::string reply_message;
    int max_us;
    if(i)
    {
        if(param.empty())
        {
            reply_message = getServerIP() + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +l ");
            sendMessage(reply_message, admin->get_clientfd());
            return;
        }
        max_us= keys(param);
        if(max_users == 0)
        {
            reply_message = getServerIP() + ERR_INVALIDMODEPARAM(admin->getnickname(), this->GetName() , "l" , param , "Invalid Number .");
            sendMessage(reply_message, admin->get_clientfd());
        }
        else if ( max_us < GetClientsNumber())
        {
            reply_message = getServerIP() + ERR_INVALIDMODEPARAM(admin->getnickname(), this->GetName() , "l" , param , "Sorry we Inform u that we Can`t set Limit lower than existing Members.");
            sendMessage(reply_message, admin->get_clientfd());
        }
        else
        {
            this->has_limit = true;
            SetMaxUsers(max_us);
            setMode("l");
            reply_message = ":" + admin->getPrefix() + " MODE " + this->GetName() + " +l " + param + "\r\n";
            sendMessage(reply_message, admin->get_clientfd());
        }
    }
    else
    {
        has_limit = false;
        reply_message = ":" + admin->getPrefix() + " MODE " + this->GetName() +  " -l " + "\r\n"; 
    }
}

void Channel::changeInviteMode(Client *admin, bool i)
{
    std::string reply_message;
    if(i)
    {
        this->invite_only = true;
        reply_message = ":" + admin->getPrefix() + "MODE" + this->name + "+i" + "\r\n";
        SetMode("i");    
    }
    else
    {
        this->invite_only = false;
        reply_message = ":" + admin->getPrefix() + "MODE" + this->name + "-i" + "\r\n";
    }
    send_to_all(reply_message);
}

void Channel::changeKeyMode(Client *admin, std::string key, bool i)
{
    std::string reply_message;
    if(i)
    {
        if(key.empty())
        {
            reply_message = ":" + getServerIP() + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +k");
            sendMessage(reply_message, admin->get_clientfd());
            return;
        }
        SetPassword(key);
        SetMode("k");
        this->has_password = true;
        reply_message = admin->getPrefix() + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " +k" );
        sendMessage(reply_message, admin->get_clientfd());
    }
    else{
        this->has_password = false;
        reply_message = admin->getPrefix() + RPL_CHANNELMODEIS(admin->getnickname(), this->GetName(), " -k" );
        sendMessage(reply_message, admin->get_clientfd());
    }

}

void Channel::changeTopicMode(Client *admin, bool i)
{
    std::string reply_message;
    if(i)
    {
        this->has_topic = true;
        SetMode("t");
        reply_message = ":" + admin->getPrefix() +  " MODE " + this->GetName() + " +t " + "\r\n";
        send_to_all(reply_message);
    }
    else
    {
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

void Channel::remove_admino(Client *admin, std::string name)
{
    std::string  reply_message;
    if (name.empty())
    {
        reply_message = getServerIP() + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +o " + name);
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
        reply_message = getServerIP() + ERR_USERNOTINCHANNEL(admin->getnickname(), name , this->GetName());
        sendMessage(reply_message, admin->get_clientfd());
    }

}

void Channel::remove_user(Client *admin)
{
    std::vector<Client*>::iterator it = std::find(invites.begin(), invites.end(), admin);
    if (it != invites.end()) {
        invites.erase(it);
    }
}


void Channel::add_admin(Client *admin, std::string name)
{
    this->operate = true;
    std::string reply_message;
    if(name.empty())
    {
        reply_message = getServerIP() + ERR_NEEDMOREPARAMS(admin->getnickname(), "MODE" + " +o " + name);
        sendMessage(reply_message, admin->get_clientfd());
        return;
    }
    Client *user = GetUser(name);
    if(user)
    {
        admins.push_back(user);
        reply_message = admin->getPrefix() + " MODE " + this->GetName() + "+o" + name + "\r\n";
        send_to_all(reply_message);
    }
    else
    {
        reply_message = getServerIP() + ERR_USERNOTINCHANNEL(admin->getnickname(), name, this->GetName());
        sendMessage(reply_message, admin->get_clientfd());
    }
        
}
void Channel::addUser(Client* client, std::string pass)
{
    if (!client)
     {
        std::cout << "Client pointer is null in addUser!" << std::endl;
    }

    if (has_password)
    {
        if (pass == password)
        {
            std::string rpl = client->getPrefix() + " JOIN " + name + " * :realname\r\n";
            std::cout << "madkoukin hna2\n";
            
            if (std::find(Clients.begin(), Clients.end(), client) == Clients.end())
            {
                Clients.push_back(client);
                send_to_all(rpl);
            } 
            else 
            {
                std::cerr << "Client already exists in the channel!" << std::endl;
            }
        } else {
            std::cerr << "Invalid Password" << std::endl;
        }
    }
    else
    {
        std::string rpl = ":" +  client->getPrefix() + " JOIN " + name + " * :realname\r\n";
        std::cout << "madkoukin hna\n";
        if (std::find(Clients.begin(), Clients.end(), client) == Clients.end())
        {
            Clients.push_back(client);
            send_to_all(rpl);
        } else {
            std::cerr << "Client already exists in the channel!" << std::endl;
        }
    }
}
void Server::JOIN(Client* client, std::string& line)
{
    std::string cmd , chan_name , pass;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, pass);
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
                    chan->addUser(client, pass);
        }
    }
    else 
    {
        if (client)
        {
            std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(client->getnickname(), chan_name);
            send(client->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}

void Server::PASS(Client * client , std::string & line)
{
    std::string rawline = line;
    std::istringstream ss(rawline);
    std::string cmd , passio;
    ss >> cmd;
	ss >> passio;

    line = line.substr(4);
	size_t pos = line.find_first_not_of("\t\v ");
	if(pos < line.size())
	{
		line = line.substr(pos);
		if(line[0] == ':')
			line.erase(line.begin());
	}
	if(pos == std::string::npos || line.empty()) 
    {

        std:: string msg_to_reply =  ":" + getServerIP() + ERR_NOTENOUGHPARAM(client->getnickname());
		ssendMessage(msg_to_reply , client->get_clientfd() );
		// _sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd);
    }
	else if(!client->getregistred())
	{
		std::string pass = line;
		if(pass == Password)
			client->setregistred(true);
		else
        {
            std:: string msg_to_reply =  ":" + getServerIP() + ERR_INCORPASS(client->getnickname());
		    ssendMessage(msg_to_reply , client->get_clientfd() );        }
	}
	else
    {
        std:: string msg_to_reply =  ":" + getServerIP() + ERR_ALREADYREGISTERED(client->getnickname());
		ssendMessage(msg_to_reply , client->get_clientfd() );
    }
}

void Server::NICK(Client * client , std::string & line)
{
    std::istringstream ss(line);
    std::string cmd , nick;
    ss >> cmd;
	ss >> nick;

	if (!nick.empty()) 
    {
		if (!client)
        {
			if (nick[0] == '#')
            {
				std:: string msg_to_reply =  ":" + getServerIP() + ERR_ERRONEUSNICKNAME(client->getnickname(), nick);
				ssendMessage(msg_to_reply , client->get_clientfd() );
			}
            else
            {
				std:: string msg_to_reply = ":"  + client->getPrefix() + " NICK :" + nick + "\r\n";
				client->setnickname(nick);
				for (size_t i = 0; i < Clients.size(); ++i)
                {
		            ssendMessage( msg_to_reply, Clients[i]->get_clientfd());
	            }
			}
		}
        else
        {
			std:: string msg_to_reply = ":" + getServerIP() + ERR_NICKNAMEINUSE(client->getnickname(), nick);
			ssendMessage( msg_to_reply, client->get_clientfd());
		}
	}
    else
    {
		std:: string msg_to_reply = ":" + getServerIP() + ERR_NONICKNAMEGIVEN(client->getnickname());
		ssendMessage( msg_to_reply, client->get_clientfd());
	}
}


void Server::USER(Client * client , std::string & line)
{
    std::istringstream ss(line);
    std::string username, hostname, servername, realname;
    ss >> username;
    ss >> hostname;
    ss >> servername;
    ss >> realname;

    std::cout << "\nip == " << servername << "xddd\n xdd\n";
    if (username.empty() || realname.empty() || servername.empty() || hostname.empty())
    {   
        client->sendError(client , "461" , "" , " u need to enter a username,realname,servername,hostname  to acces the server ");
    }
    else if (!client->getregistred())
    {
        client->sendError(client , "451", "" , "ERR_NOTREGISTERED1");
    }   
    else if ( client->getnickname().empty() || username.empty() || realname.empty() || servername.empty() || hostname.empty())
    {
        std::string msg_to_reply =  ":" + getServerIP() + ERR_ALREADYREGISTERED(client->getnickname());
		ssendMessage(msg_to_reply , client->get_clientfd() );
    }   
    else
    {
        client->setusername(username);
        client->setservername(servername);
        client->setrealname(realname);
        client->sethostname(hostname);  
    }
    if (client->getregistred() && !client->getusername().empty() && !client->getrealname().empty() && !client->getservername().empty() && !client->gethostname().empty() && !client->getnickname().empty())
    {
        client->setlogedstatus(true);
    }
            
    if (client->getregistred() && client->getnickname().size() > 0 && client->getusername().size() > 0)
        sendWelcome(client->get_clientfd());
}

void Server::KICK(Client *admin,  std::string &line)
{
    std::istringstream ss(line);
    std::string cmd, chan_name, nick, reason;
    ss >> cmd;
    ss >> chan_name;
    ss >> nick;
    std::getline(ss, reason);

    Channel *chan = getChannel(chan_name);
    Client  *clio = getClientname(nick);

    std::cout << " Ara wa7ed kick hna --#%& " << std::endl;
    std::string reply_message;

    if (chan)
    {
        if(clio)
        {
            if(chan->is_inChannel(clio))
            {
                if(chan->is_Admin(clio))
                {
                    chan->remove_admin(clio);
                }

                if(chan->is_Invited(clio))
                {
                    chan->remove_Invited(clio);
                } 
                std::string msg_to_reply = ":" + admin->getPrefix() + " KICK " + chan->GetName() + clio->getnickname() + " " + reason + "\r\n" ;
                chan->send_to_all(reply_message);
                chan->remove_user(clio);
            }
            else 
            {
                std::string msg_to_send = ":" +  getServerIP() + ERR_NOTONCHANNEL(nick, chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }


            if (chan->is_Admin(admin) && !chan->is_Admin(clio))
            {
                if (chan->is_inChannel(clio))
                {
                    if (chan->is_Invited(clio))
                        chan->remove_Invited(clio);
                    std::string msg_to_reply = ":" + admin->getPrefix() + " KICK " + chan_name +  " " + clio->getnickname() + " " + reason + "\r\n";
                    chan->send_to_all(msg_to_reply);
                    chan->remove_user(clio);
                } 
                else
                {
                    std::string msg_to_reply =  ":" + getServerIP() + ERR_NOTONCHANNEL(clio->getnickname(), chan_name);
                    ssendMessage( msg_to_reply , admin->get_clientfd());
                }
            }
            else
            {
                std::string msg_to_reply = ":" + getServerIP() + ERR_CHANOPRIVSNEEDED(admin->getnickname(), chan_name);
                ssendMessage( msg_to_reply  ,admin->get_clientfd() );
            }

        }
        else
        {
            std::string msg_to_send = ":" +  getServerIP()  + ERR_NOTONCHANNEL(nick, chan_name);
             send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
    else
    {
        std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
        send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
    }

}


void Server::INVITE(Client *admin, std::string &line){
    std::cout << " Ara wa7ed Invite hna --#%& " << std::endl;
    std::string reply_message;
    std::string cmd, nick , chan_name;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> nick;
    std::getline(ss, chan_name);

    Client *clio = getClientname(nick);
    Channel *chan = getChannel(chan_name);

    if (clio)
    {
        if (chan)
        {
            if(chan->is_Admin(admin))
            {
                if (chan->is_inChannel(admin))
                {
                    chan->invites.push_back(admin);
                    std::string msg_to_reply = getServerIP() + RPL_INVITING(admin->getnickname() , clio->getnickname() , chan_name);
                    ssendMessage(msg_to_reply, admin->get_clientfd());

                    std::string msg_to_reply = ":" + admin->getPrefix() + " INVITE " + clio->getnickname() + " " + chan->GetName() + "\r\n" ;
                    ssendMessage(msg_to_reply, clio->get_clientfd());
                }
                else
                {
                    std::string msg_to_reply = getServerIP() + ERR_USERONCHANNEL(admin->getnickname() , clio->getnickname(), chan_name);
                    ssendMessage(msg_to_reply, admin->get_clientfd());
                }
            }
            else
            {
                std::string msg_to_reply = ":" + getServerIP() + ERR_CHANOPRIVSNEEDED(admin->getnickname(), chan_name);
            }
        }
        else 
        {
                std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
    else
    {
        if (admin)
        {
            std::string msg_to_send = ":" + getServerIP() + ERR_NOSUCHNICK(admin->getnickname(), nick);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}

void Server::TOPIC(Client *admin, std::string &line)
{
    std::cout << " Ara wa7ed Topic hna --#%& " << std::endl;
    std::istringstream ss(line);
    std::string cmd , chan_name , topic;
    std::string reply_message;
    std::string error_message;

    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, topic);

    Channel *chan = getChannel(chan_name);
    if (chan)
    {
        if(topic.find(':') == std::string::npos)
        {
            if (chan->is_inChannel(admin))
            {
                if(chan->get_has_topic())
                {
                    if(!topic.empty())
                    {
                        chan->SetTopic(topic);

                        reply_message = getServerIP() + RPL_TOPIC(admin->getnickname(), chan->GetName(), topic);
                        ssendMessage(reply_message, admin->get_clientfd());

                        reply_message = getServerIP() + RPL_TOPICWHOTIME(admin->getnickname(), chan->GetName(), admin->getnickname(), std::to_string(chan->get_topictime()));
                        ssendMessage(reply_message, admin->get_clientfd());
                    }
                }
                else
                {
                    reply_message = getServerIP() + RPL_NOTOPIC(admin->getnickname(), chan->GetName());
                    ssendMessage(reply_message, admin->get_clientfd());
                }
            }
            else 
            {
                std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }
            return;
        }


        if(chan->is_Admin(admin))

            if (chan->get_modes().find('t') == std::string::npos)
            {
                chan->SetTopic(topic);
                reply_message = ":" + admin->getPrefix()   +  " TOPIC " + chan->GetName() + " " + topic + "\r\n";
                chan->send_to_all(reply_message);
            }
            else
            {
                   std::string msg_to_send = ":" +  getServerIP()  + ERR_INVALIDMODEPARAM(admin->getnickname(), chan_name, "TOPIC", topic, "Channel Topic Restrection Are On");
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }

        else
        {
            std::string msg_to_send = ":" +  getServerIP()  + ERR_CHANOPRIVSNEEDED(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
    else
    {
        if (admin)
        {
             std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}

void Server::PART(Client *admin, std::string &line)
{
    std::cout << " Ara wa7ed Part hna --#%& " << std::endl;
    std::string cmd , chan_name , reason;

    std::istringstream ss(line);
    ss >> cmd;
    ss >> chan_name;
    std::getline(ss, reason);
    Channel *chan = getChannel(chan_name);
    std::string reply_message;

    if (chan)
    {
        if (chan->is_inChannel(admin))
        {

            if(chan->is_Admin(admin))
                chan->remove_admin(admin);
            if(admin)
            {
                reply_message = ":" + admin->getPrefix() + " PART " + chan->GetName() + " :leaving channel\r\n";
                chan->send_to_all(reply_message);
                chan->remove_user(admin);
            }
        }
        else
        {
            std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }

    }
    else
    {
        if (admin)
        {
            std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
            send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}
// // Special Considerations: !!!!!!!!!!!
// // If the last operator leaves a channel (e.g., by using PART), the channel may be left without any operator unless another user is automatically promoted to operator. In some IRC implementations, this happens to ensure channel continuity.
// // An operator leaving via PART does not override channel restrictions like invite-only mode (+i) or password protection (+k). Rejoining the channel would still require them to adhere to those rules.
// /////////////////////////////////////


bool Channel::onChannel(Client *admin)
{
    if(std::find(Clients.begin(), Clients.end(), admin) != Clients.end())
        return true;
    return false;
}

void Server::PRIVMSG(Client *admin, std::string &line)
{   
    std::string cmd , nicknameiichannel , msg;
    std::istringstream ss(line);
    ss >> cmd;
    ss >> nicknameiichannel;
    std::getline(ss, msg);

    std::cout << " Ara wa7ed Privmsg hna --#%& " << std::endl;
    if (nicknameiichannel[0] == '#')
    {
        Channel *chan = getChannel(nicknameiichannel);
        if (chan)
        {
            if (!chan->onChannel(admin))
            {
                std::string error_msg = ERR_NOTONCHANNEL(admin->getnickname(), chan->GetName());
                send(admin->get_clientfd(), error_msg.c_str(), error_msg.length(), 0);
            }

            if(msg[0] == ':')
            {
                std::string msg_to_send = ":" + admin->getnickname() + "!" + admin->getusername() + "@" + admin->getIPaddress() + " PRIVMSG " + chan->GetName() + " :" + msg + "\r\n";
                chan->send_to_all(msg_to_send);
            }
        }
        else
        {
            if (admin)
            {
                // _server->getIpaddress() is the same as admin->getIPaddress()
                std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(admin->getnickname(), nicknameiichannel);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
            }

        }
    }
    else 
    {
        Client *clio = getClientname(nicknameiichannel);
        if (clio)
        {
            std::string msg_to_send = ":" + admin->getnickname() + "!" + admin->getusername() + "@" + admin->getIPaddress() + " PRIVMSG " +  clio->getnickname() + " :" + msg + "\r\n";
            std::string	er = ("Cannot send message: client is not active or socket is invalid");
            if (clio->get_clientfd() <= 0)
            {
                ssendMessage(er, admin->get_clientfd());
            }
            else 
            {
                ssendMessage(msg_to_send , clio->get_clientfd());
            }
        }
        else 
        {
            if (admin)
            {
                // _server->getIpaddress()
                std::string msg_to_send = ":" + getServerIP() + ERR_NOSUCHNICK(admin->getnickname(), nicknameiichannel);
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
                std::string msg_to_send = ":" +  getServerIP()  + ERR_NOSUCHCHANNEL(admin->getnickname(), chan_name);
                send(admin->get_clientfd(), msg_to_send.c_str(), msg_to_send.length(), 0);
        }
    }
}

void Server::QUIT(Client *clio, std::string &line)
{
    std::string cmd, reason;
    std::istringstream ss(line);

    // Extract the first word (command) and the rest (reason)
    ss >> cmd;
    std::getline(ss, reason); // Get the rest of the line as the reason

    // Trim leading spaces from the reason
    size_t start_pos = reason.find_first_not_of(' ');
    if (start_pos != std::string::npos)
        reason = reason.substr(start_pos);

    // Add default quit message if no reason is provided
    if (reason.empty())
        reason = "Quit";

    // Ensure the reason starts with a colon
    if (reason[0] != ':')
        reason = ":" + reason;

    // Notify all channels and clients
    for (size_t i = 0; i < Channels.size(); ++i)
    {
        // Check if the client is in the channel as a member or admin
        if (Channels[i]->GetClientInChannel(clio->getnickname()) || Channels[i]->getOperator(clio->getnickname()))
        {
            // Remove the client/admin from the channel
            Channels[i]->remove_user(clio);
            Channels[i]->remove_admin(clio);

            // Delete the channel if it's empty
            if (Channels[i]->GetClientsNumber() == 0)
            {
                Channels.erase(Channels.begin() + i);
                --i; // Adjust index after erase
            }
            else
            {
                // Notify remaining clients in the channel
                std::string msg_to_send = ":" + clio->getnickname() + "!~" + clio->getusername() + "@localhost QUIT " + reason + "\r\n";
                Channels[i]->send_to_all(msg_to_send);
            }
        }
    }
}


void Server::MODE( Client *client, std::string &line)
{
    std::string cmd , chan_name, modes, arg, reply_message;
    std::istringstream ss(line);

    ss >> cmd >> chan_name >> modes >> arg;
    Channel *chan = getChannel(chan_name);
    if(chan)
    {
        if(!modes.empty())
        {
            if(chan->is_Admin(client))
            {
                chan->valid_mode(client, modes, arg);
            }
            else
            {
                if(client)
                    reply_message = ":" + getServerIP() + ERR_CHANOPRIVSNEEDED(client->getnickname(), chan_name);
            } 
        }
        else
        {
            if(!chan->get_topic().empty())
                chan->rpl_topic(client, "fda");
            chan->rpl_list(client);
            chan->rpl_mode(client);
        }
    }
    else
    {
            if(client)
            ssendMessage(":" + getServerIP() + ERR_NOSUCHCHANNEL(client->getnickname(), chan_name), client->get_clientfd());
    }
}
