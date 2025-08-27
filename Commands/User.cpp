#include "../Client.hpp"
#include "../Messages.hpp"

/*
    https://modern.ircdocs.horse/#user-message

ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | YES          | Not enough fields in USER command
ERR_ALREADYREGISTERED (462)   | YES          | USER after registration already completed

*/                       
void Client::do_user(std::istringstream &iss, Server &srv, Client &c)
{
    if(!c.password)
    { 
        Msg::ERR_PASSWDMISMATCH(srv, c);
        c.toDisconnect = true;
        return;
    }

    if(c.registered == true)
    {
        Msg::ERR_ALREADYREGISTERED(srv, c);
        return;
    }

    std::string user, hostname, servername, realname;

    if (!(iss >> user >> hostname >> servername)) {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "USER");
        return;
    } 
    
    if (iss.peek() == ' ') iss.get();

    std::getline(iss, realname);
    while(!realname.empty() && (realname[0]== ' ' || realname[0]== ':')) realname.erase(0,1); 
    if(realname.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "USER");
        return;
    }

    c.user = user;
    c.realname = realname;
    c.check_registration(srv);
    return;
}