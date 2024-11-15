/////////////////////////////////////////////
            // ZAKARIA PART
/////////////////////////////////////////////


// / Authentification Commands :

// // PASS :
// void Server::Pass_func(int fd, std::string cmd)
// {
//     int position;
//     std::string pass;

//     Client *clio =  getClient(fd);

//     position = cmd.find_first_not_of("\t\v ");
//     if (cmd.empty())
//     {
//         // send a response ----> ERR_NOTENOUGHPARAM;
//     }
//     else if (!clio->getregistred())
//     {
//         pass = cmd;
//         if (pass == Password)
//         {
//             clio->setregistred(true);
//         }
//         // else
//         // send a response ---->  ERR_INCORPASS;
//     }
//     // else 
//     // send response ---->  ERR_ALREADYREGISTERED;
// }

// bool Server::Valid_nick_name(std::string& nickname)
// {
//     int i = 0;
//     if (!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
//         return false;
//     while(i < nickname.size() )
//     {
//         if (!std::isalnum(nickname[i]) && nickname[i] != '_')
//             return false;
//     }
//     return true;
// }

// bool Server::Nick_name_already_used(std::string &nickname)
// {
//     int i = 0;
//     while(i < this->Clients.size())
//     {
//         if (this->Clients[i].getnickname() == nickname)
//             return true;
//         return false;
//     }
// }


// void Server::Nick_func(int fd, std::string cmd)
// {
//     int position;
//     std::string granpa; 
//     std::string used;
//     Client *clio = getClient(fd);
//     if (cmd.empty())
//     {
//         // send a response ----> ERR_NOTENOUGHPARAM;
//     }
//     if (Nick_name_already_used(cmd) && clio->getnickname() != cmd)
//     {
//         used = "212";
//         if(clio->getnickname().empty())
//             clio->setnickname(used);
//         // send response -----> ERR_NICKINUSE;
//             return;
//     }
//     if (!Valid_nick_name(cmd))
//     {
//         // send response -----> ERR_ERRONEUSNICK;
//         return;
//     }
//     else
//     {
//         if (clio && clio->getregistred())
//         {
//             granpa = clio->getnickname();
//             clio->setnickname(cmd);
//             if(!granpa.empty() && granpa != cmd)
//             {
//                 if(granpa == "212" && !clio->getusername().empty())
//                 {
//                     clio->setlogedstatus(true);
//                     // send response -----> RPL_CONNECTED;
//                     // send response -----> RPL_NICKCHANGE;
//                 }
//                 else
//                     // send response -----> RPL_NICKCHANGE;
//                 return;
//             }
//         }
//         // else if (clio && !clio->getregistred())
//             // send response -----> ERR_NOTREGISTERED;
//     }
//     if (clio && clio->getregistred() && !clio->getusername().empty() && !clio->getnickname().empty() && clio->getnickname() != "212" && !clio->getlogedstatus()) 
//     {
//         clio->setlogedstatus(true);
//         // send response -----> RPL_CONNECTED;
//     }
// }

// void Server::User_func(int fd, std::string cmd)
// {
//     Client *clio = getClient();
//     // if (this line is used to maintaine the flag condition )
//     if (!clio || !clio->getregistred())
//         // send response -------> ERR_NOTREGISTERED;
//     else if (clio && !clio->getusername().empty())
//         // send response -------> ERR_ALREADYREGISTERED;
//     else 
//         clio->setusername(cmd)
//         if (clio && clio->getregistred() && !clio->getusername().empty() && !clio->getnickname().empty() && clio->getnickname() != "212" && !clio->getlogedstatus())
//         clio->setlogedstatus(true)l
//         // send response ------>  RPL_CONNECTED;

// }











/////////////////////////////////////////////
            // OTHMAN PART
/////////////////////////////////////////////

// void Server::executing_commands(int fd, std::string Cmd)
// {
//     if(Operator_status == 1)
//     {
//         // operator priveleges :
//         if (Cmd.compare("KICK") == 0 || Cmd.compare("kick") == 0)
//             // kick_func();  
//         else if (Cmd.compare("INVITE") == 0 || Cmd.compare("invite") == 0) 
//             // invite_func();
//         else if (Cmd.compare("MODE") == 0 || Cmd.compare("mode") == 0) 
//             // mode_func(); 
//         else if (Cmd.compare("TOPIC") == 0 || Cmd.compare("topic") == 0)
//             // topic_func();
//         else if (Cmd.compare("JOIN") == 0 || Cmd.compare("join") == 0) 
//             // join_func();
//         else if (Cmd.compare("PRIVEMSG") == 0 || Cmd.compare("privemsg") == 0) 
//             // privemsg_func();
//         else if (Cmd.compare("NICK") == 0 || Cmd.compare("nick")  == 0) 
//             // nick_func(); 
//         else if (Cmd.compare("USER") == 0  || Cmd.compare("user") == 0)
//             // user_func();
//         else if (Cmd.compare("PASS") == 0 || Cmd.compare("pass") == 0) 
//             // pass_func();
//     }
//     else 
//     { 
//         // normal User priveleges :
//         if (Cmd.compare("JOIN") == 0 || Cmd.compare("join") == 0) 
//             // join_func();
//         else if (Cmd.compare("PRIVEMSG") == 0  || Cmd.compare("privemsg") == 0) 
//             // privemsg_func()
//         else if (Cmd.compare("NICK") == 0 || Cmd.compare("nick")  == 0) 
//             // nick_func(); 
//         else if (Cmd.compare("USER") == 0  || Cmd.compare("user") == 0)
//             // user_func();
//         else if (Cmd.compare("PASS") == 0 || Cmd.compare("pass") == 0) 
//             // pass_func();
//     }
// }