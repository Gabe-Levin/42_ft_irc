#include "../Client.hpp"
#include "../Messages.hpp"

void Client::do_quit(std::istringstream &iss, Server &srv, Client &c)
{
    std::string reason;
    iss >> reason;
    std::string rest;
    std::getline(iss, rest);
    
    while(!reason.empty() && reason[0] == ' ') reason.erase(0,1);
    if(reason[0] == ':')
    {
        reason.erase(0,1);
        reason = reason + rest;
    }

    Msg::BROADCAST_QUIT(srv, c, reason);
    c.toDisconnect = true;
}
