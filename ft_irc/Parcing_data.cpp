#include "Parcing_data.hpp"

Buffer::Buffer()
{
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
    }
    return *this;
}

void Buffer::Parcing_core(std::string &buffer)
{
    split_buffer_from_nc(buffer);
    print_parsed_data();
}

void Buffer::print_parsed_data() 
{
        std::cout << "Command: " << Cmd << "\n"
                  << "Argument: " << Arg << "\n"
                  << "Message: " << Msg << "\n";
}

void Buffer::split_buffer_from_nc(const std::string &buffer)
{
    std::istringstream ss(buffer);
    buf = buffer;
    ss >> Cmd;
    trim(Cmd);                
    ss >> Arg;
    trim(Arg);                  
    std::getline(ss, Msg); 
    trim(Msg);             
}

void Buffer::trim(std::string &str)
{
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}

std::string Buffer::get_target()
{
    size_t pos = this->get_cmd().find(" ");
    if(pos == std::string::npos)
        return "";
    std::string target = this->get_cmd().substr(pos + 1);
    size_t pos2 = target.find(" :");
    if(pos2 != std::string::npos)
        target = target.substr(0, pos2);
    return target;
}