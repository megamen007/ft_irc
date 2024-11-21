#pragma once

#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>

class Buffer
{
    private:
        std::string buf;
        std::string Cmd;
        std::string Arg;
        std::string Msg;
        std::vector<std::string> valid_commands;
        std::vector<char> valid_mode_flags;
 

    public:
        Buffer();
        ~Buffer();
        Buffer(const Buffer& newBuffer);
        Buffer &operator=(Buffer const &newBuffer);

    // Getters 
        std::string get_arg();
        std::string get_cmd();
        std::string get_msg();
        

    // members functions
        bool is_initial_hexchat_handshake(std::string &buffer);
        void check_Client_is_authentifacted(std::string &buffer);
        void Parcing_core(std::string &buffer);
        void checking_mode_args(const std::string &args);
        void print_parsed_data();
        void Parcing_from_nc(std::string &buffer);
        void Parcing_from_hexchat(std::string &buffer);
        void split_buffer_from_hexchat(const std::string &buffer);
        void split_buffer_from_nc(const std::string &buffer);
        void checking_core(std::string &cmd, std::string &arg);
        void checking_command(std::string &cmd);
        void checking_args(std::string &args);
        void trim(std::string &str);

        static bool received_pass;
        static bool received_nick;
        static bool received_user;

};