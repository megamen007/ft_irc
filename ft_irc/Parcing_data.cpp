
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

void Parcing_core(std::string &buffer)
{
    Parcing_from_nc(buffer);
    Parcing_from_hexchat(buffer);   
}


void Parcing_from_nc(std::string &buffer)
{
    split_buffer_from_nc(buffer);
    checking_core(cmd, arg);
}

void Parcing_form_hexchat(std::string &buffer)
{
    split_buffer_from_hexchat(buffer);
    checking_core(cmd, arg);
}

void checking_core(std::string &cmd, std::string &arg)
{
    checking_command(cmd);
    checking_args(arg);
}

void checking_command(std::string &cmd)
{

}

void checking_args(std::string &args)
{

}