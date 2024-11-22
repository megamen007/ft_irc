/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 05:39:28 by mboudrio          #+#    #+#             */
/*   Updated: 2024/11/22 22:03:58 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

// Canonical Form:

Channel::Channel(){
    name = "default";
    invite_only = false;
    has_password = false;
    has_topic = false;
    has_limit = false;
    operate = false;
}

Channel::Channel(const std::string& name) : name(name) 
{
    invite_only = false;
    has_password = false;
    has_topic = false;
    has_limit = false;
    operate = false;
}

Channel &Channel::operator=(const Channel &src){
    if(this == &src)
        return *this;
    name = src.name;
    topic = src.topic;
    modes = src.modes;
    password = src.password;
    invite_only = src.invite_only;
    has_password = src.has_password;
    has_topic = src.has_topic;
    has_limit = src.has_limit;
    operate = src.operate;
    max_users = src.max_users;
    Clients = src.Clients;
    admins = src.admins;
    invites = src.invites;
    return *this;
}

Channel::Channel(Channel const &src){
    *this = src;
}

Channel::~Channel(){
    Clients.clear();
    admins.clear();
    invites.clear();
}

void Channel::SetName(std::string name){
    this->name = name;
}

void Channel::SetTopic(std::string topic){
    this->topic = topic;
    has_topic = true;
}

void Channel::SetPassword(std::string password){
    this->password = password;
    has_password = true;
}

void Channel::SetMaxUsers(int max){
    this->max_users = max;
    has_limit = true;
}

void Channel::SetTime(std::string time)
{
    this->creation_time = time;
}

void Channel::set_has_password(bool has_password)
{
    this->has_password = has_password;
}


void Channel::setbuffer(std::string message, int destination_fd){
    int i;
    if((i = send(destination_fd, message.c_str(), message.size(), 0)) == -1)
        throw std::runtime_error("send failed");
    if(i != (int)message.size())
        throw std::runtime_error("send failed: not all bytes sent");
}

std::string Channel::GetName(){
    return name;
}

std::string Channel::get_time(){
	return this->creation_time;
}

bool Channel::get_has_password()
{
    return this->has_password;
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


Client *Channel::GetUser(std::string name){
    for(size_t i = 0; i < Clients.size(); ++i){
        if(name == Clients[i]->getnickname())
            return Clients[i];
    }
    return NULL;
}

std::string Channel::GetUserInfo(Client *admin, bool i){
    if(i)
        return ":" + admin->getnickname() + "!" + admin->getusername() + "@" + admin->getservername() + " ";
    else
        return ":" + admin->getservername() + " ";
}


