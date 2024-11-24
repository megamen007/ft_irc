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
        void Parcing_core(std::string &buffer);
        void print_parsed_data();
        void split_buffer_from_nc(const std::string &buffer);
        void trim(std::string &str);
        std::string get_target();

};