#include "../Client.hpp"
#include "../Messages.hpp"

/*
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | KICK without <channel> or <nick>
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_NOTONCHANNEL (442)        | NO          | User is not on channel
ERR_USERNOTINCHANNEL (441)    | NO          | Target user is not in channel
ERR_CHANOPRIVSNEEDED (482)    | NO          | User is not channel operator

*/

void Client::do_kick(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel_name;
    iss >> channel_name;

    Channel* channel = Channel::find_channel(channel_name, srv);
    if(channel == NULL)
    {                
        c.outbuf += "Could not find reference channel: " + channel_name + "\r\n";
        return;
    }
    Client * op = (*channel).find_operator(c.nick);
    if(op == NULL)
    {
        c.outbuf += "Sorry bud, you are not an operator. \r\n";
        return;
    }
    
    std::string nick;
    iss >> nick;
    if((*channel).kick_client(nick))
    {
        c.outbuf += "User " + nick + " could NOT be found in: " + channel_name + "\r\n";
        return;
    }
    else
        c.outbuf += "User " + nick + " has been kicked from " + channel_name + "\r\n";
    return;
}