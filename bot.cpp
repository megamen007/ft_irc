/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otelliq <otelliq@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 15:38:56 by otelliq           #+#    #+#             */
/*   Updated: 2024/12/13 15:59:01 by otelliq          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bot.hpp"


void Bot::Bot_call(std::string param){
    std::string reply_message;
    if (param == "joke")
        reply_message = jokes[rand() % jokes.size()];
    else if (param == "insult")
        reply_message = insults[rand() % insults.size()];
    else
        reply_message = "I don't know what you mean by " + param;
}