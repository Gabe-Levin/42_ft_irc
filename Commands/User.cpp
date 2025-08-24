#include "../Client.hpp"
#include "../Messages.hpp"

/*
    https://modern.ircdocs.horse/#user-message

ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | YES          | Not enough fields in USER command
ERR_ALREADYREGISTERED (462)   | YES          | USER after registration already completed

*/                       
//TODO: User should have a specific format (ex. "USER myuser 0 * :John Smith")
void Client::do_user(std::istringstream &iss, Server &srv, Client &c)
{

    if(c.registered == true)
    {
        Msg::ERR_ALREADYREGISTERED(srv, c);
        return;
    }

    std::string username, hostname, servername, realname;

    if (!(iss >> username >> hostname >> servername)) {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "USER");
        return;
    } 
    
    if (iss.peek() == ' ') iss.get();
    if (iss.peek() == ':') iss.get();

    std::getline(iss, realname);
    
    while(!realname.empty() && realname[0]== ' ') realname.erase(0,1); 
    
    if(realname.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "USER");
        return;
    }

    c.user = user;
    c.realname = realname;
    return;
}