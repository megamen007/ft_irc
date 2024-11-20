#include "Parcing_data.hpp"

// Parcing logic 
/*{
    #first_Part:
    split the buffer into 
    Command 
    Arg
    Message 

    #Second_Part:
    check Command 
    check Arg

    #third_Part
    if everything is ok pass the command and the arg to othman .
}*/

Buffer::Buffer()
{
    valid_commands.push_back("NICK");
    valid_commands.push_back("USER");
    valid_commands.push_back("PASS");
    valid_commands.push_back("KICK");
    valid_commands.push_back("MODE");
    valid_commands.push_back("JOIN");
    valid_commands.push_back("PRIVMSG");
    valid_commands.push_back("TOPIC");
    valid_commands.push_back("INVITE");

    valid_mode_flags.push_back('i');
    valid_mode_flags.push_back('t');
    valid_mode_flags.push_back('k');
    valid_mode_flags.push_back('o');
    valid_mode_flags.push_back('l');
}
Buffer::~Buffer()
{}


// getters 

std::string Buffer::get_arg()
{
    return this->Arg;
}
std::string Buffer::get_cmd()
{
    return this->Cmd;
}
std::string Buffer::get_msg()
{
    return this->Msg;
}


void Buffer::Parcing_core(std::string &buffer)
{
    if (is_initial_hexchat_handshake(buffer))
        Parcing_from_hexchat(buffer);
    else 
        Parcing_from_nc(buffer);

    print_parsed_data();
}

Buffer::Buffer(const Buffer& newBuffer)
{
    *this = newBuffer;
}

Buffer &Buffer::operator=(Buffer const &newBuffer)
{
    if (this != &newBuffer)
    {
        this->Cmd = newBuffer.Cmd;
        this->Arg = newBuffer.Arg;
        this->Msg = newBuffer.Msg;
        this->valid_commands = newBuffer.valid_commands;
        this->valid_mode_flags = newBuffer.valid_mode_flags;
    }
    return *this;
}

void Buffer::print_parsed_data() 
{
        std::cout << "Command: " << Cmd << "\n"
                  << "Argument: " << Arg << "\n"
                  << "Message: " << Msg << "\n";
}
bool Buffer::is_initial_hexchat_handshake(std::string &buffer)
{
    return(buffer.find("CAP LS") != std::string::npos );
}

void Buffer::split_buffer_from_nc(const std::string &buffer)
{
    std::istringstream ss(buffer);
    ss >> Cmd;
    trim(Cmd);                
    ss >> Arg;
    trim(Arg);                  
    std::getline(ss, Msg); 
    trim(Msg);             
}

void Buffer::split_buffer_from_hexchat(const std::string &buffer)
{
    std::istringstream ss(buffer);
    ss >> Cmd;
    trim(Cmd);                
    ss >> Arg;
    trim(Arg);                  
    std::getline(ss, Msg); 
    trim(Msg);             
}

void Buffer::Parcing_from_nc(std::string &buffer)
{
    split_buffer_from_nc(buffer);
    checking_core(Cmd, Arg);
}

void Buffer::Parcing_from_hexchat(std::string &buffer)
{
    split_buffer_from_hexchat(buffer);
    checking_core(Cmd, Arg);
}

void Buffer::checking_core(std::string &cmd, std::string &arg)
{
    checking_command(cmd);
    if (cmd == "MODE")
        checking_mode_args(arg);
    // checking_args(arg);
}

void Buffer::checking_command(std::string &cmd)
{
    if(std::find(valid_commands.begin(), valid_commands.end(), cmd) == valid_commands.end())
    {
        std::cerr << "Invalid command: " << cmd << std::endl;
        return;
    }
}

void Buffer::checking_mode_args(const std::string &args)
{
    if (args.empty())
    {
        std::cerr << "Arguments cannot be empty for command: " << Cmd << std::endl;
        return ;
    }
    for (std::string::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        if (std::find(valid_mode_flags.begin(), valid_mode_flags.end(), *it) == valid_mode_flags.end())
        {
            std::cerr << "Invalid MODE flag: " << *it << std::endl;
            return;
        }
    }
}

void Buffer::checking_args(std::string &args)
{
        if (args.empty())
        {
            std::cerr << "Arguments cannot be empty for command: " << Cmd << std::endl;
            return ;
        }
}

void Buffer::trim(std::string &str)
{
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}