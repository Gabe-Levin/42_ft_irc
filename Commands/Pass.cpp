#include "../Client.hpp"

/*
    https://modern.ircdocs.horse/#pass-message
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | PASS without password
ERR_ALREADYREGISTERED (462)   | NO          | PASS after registration already completed
ERR_PASSWDMISMATCH (464)      | NO          | Wrong password (disconnect after sending error)

*/

void Client::do_pass(std::istringstream &iss, Server &srv, Client &c)
{
    std::string pass;
    iss >> pass;
    if(pass == srv._password)
    {
        c.password = true;
        return;
    }
    else
    { 
        c.outbuf += "Wrong password. Reseting buffer.\r\n";
        c.toDisconnect = true;
        return;
    }
}