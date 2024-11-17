/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboudrio <mboudrio@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 05:39:28 by mboudrio          #+#    #+#             */
/*   Updated: 2024/09/11 06:23:06 by mboudrio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

// Canonical Form:

Channel::Channel()
{
    this->name = "";
    this->topic = "";
    this->invite_status = false;
    this->limit = 0;
    this->key = 0;
    this->creation_time = "";
}

Channel &Channel::operator=(const Channel &src)
{
    if (this != &src)
    {
        this->name = src.name;
        this->topic = src.topic;
        this->invite_status = src.invite_status;
        this->limit = src.limit;
        this->key = src.key;
        this->creation_time = src.creation_time;
        
    }
    return *this;
}

Channel::Channel(Channel const &src)
{
    *this = src;
}

Channel::~Channel()
{}

// Channel setters:

void Channel::SetInvitOnly(int invit_only)
{
    this->invite_status = invit_only;
}
void Channel::SetTopic(int topic)
{
    this->topic = topic;
}
void Channel::SetTime(std::string time)
{
    this->creation_time = time;
}
void Channel::SetKey(int key)
{
    this->key = key;
}
void Channel::SetLimit(int limit)
{
this->limit = limit;
}

void Channel::SetPassword(std::string password)
{
    this->password = password;
}
void Channel::SetName(std::string name)
{
    this->name = name;
}


// Channel getters:

int Channel::GetInvitOnly()
{
    return this->invite_status;
}
std::string Channel::GetTopic()
{
    return this->topic;
}
int Channel::GetKey()
{
    return this->key;
}
int Channel::GetLimit()
{
    return this->limit;
}

std::string Channel::GetPassword()
{
    return this->password;
}
std::string Channel::GetName()
{
    return this->name;
}
std::string Channel::GetTime()
{
    return this->creation_time;
}

// Adding functions:

void Channel::add_Client(Client &newClient)
{
    Clients.push_back(newClient);
}

void Channel::add_Operator(Client &newOperator)
{
    Operators.push_back(newOperator);
}

