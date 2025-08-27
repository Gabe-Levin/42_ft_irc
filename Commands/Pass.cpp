#include "../Client.hpp"
#include "../Messages.hpp"

/*
    https://modern.ircdocs.horse/#pass-message
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | YES          | PASS without password
ERR_ALREADYREGISTERED (462)   | YES          | PASS after registration already completed
ERR_PASSWDMISMATCH (464)      | YES          | Wrong password (disconnect after sending error)

*/

void Client::do_pass(std::istringstream &iss, Server &srv, Client &c)
{
    std::string pass;
    iss >> pass; 
    std::string rest;
    std::getline(iss, rest);

    while(!pass.empty() && pass[0] == ' ') pass.erase(0,1);
    if(pass[0] == ':')
    {
        pass.erase(0,1);
        pass = pass + rest;
    }

    if(c.registered == true)
    {
        Msg::ERR_ALREADYREGISTERED(srv, c);
    }
    else if(pass.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "PASS");
    }
    else if(pass == srv._password)
    {
        c.password = true;
    }
    else
    { 
        Msg::ERR_PASSWDMISMATCH(srv, c);
        c.toDisconnect = true;
    }
    return;
}