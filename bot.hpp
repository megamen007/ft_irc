/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 15:47:15 by otelliq           #+#    #+#             */
/*   Updated: 2024/12/13 15:54:36 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "server.hpp"
#include "channel.hpp"
#include "Client.hpp"

class Bot
{
    public::
    std::vector<std::string> jokes = {};
    std::vector<std::string> insults = {};
    Bot();
    ~Bot();
    void Bot_call(std::string param);
};

