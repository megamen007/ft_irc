#pragma once

#include <iostream>
class Buffer
{
    private:
        std::string Cmd;
        std::string Arg;
        std::string Msg;

    public:
        Buffer();
        ~Buffer();
        Buffer(const Buffer& newBuffer);
        Buffer &operator=(Buffer const &newBuffer);

    // members functions
        void Parcing_core(std::string &buffer);
        void Parcing_from_nc(std::string &buffer);
        void Parcing_form_hexchat(std::string &buffer);
        void checking_core(std::string &cmd, std::string &arg);
        void checking_command(std::string &cmd);
        void checking_args(std::string &args);

};