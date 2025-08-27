#include "../Client.hpp"
#include "../Messages.hpp"

/*
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | YES          | KICK without <channel> or <nick>
ERR_NOSUCHCHANNEL (403)       | YES          | Channel does not exist
ERR_NOTONCHANNEL (442)        | YES          | User is not on channel
ERR_USERNOTINCHANNEL (441)    | YES          | Target user is not in channel
ERR_CHANOPRIVSNEEDED (482)    | YES          | User is not channel operator

*/

void Client::do_kick(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel_name, nick, reason;
    iss >> channel_name;
    iss >> nick;
    if(channel_name.empty() || nick.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "KICK");
        return;
    }

    Channel* channel = Channel::find_channel(channel_name, srv);
    if(channel == NULL)
    {
        Msg::ERR_NOSUCHCHANNEL(srv, c, channel_name);
        return;
    }

    if(!(*channel).is_on_client_list(nick))
    {
        Msg::ERR_NOTONCHANNEL(srv, c, *channel);
        return;
    }

    Client * op = (*channel).find_operator(c.nick);
    if(op == NULL)
    {
        Msg::ERR_CHANOPRIVSNEEDED(srv, c, *channel);
        return;
    }
    
    while (iss.peek() == ' ' || iss.peek() == ':') iss.get();
    std::getline(iss, reason);
    Msg::KICK(srv, c, *channel, nick, reason);

    (*channel).kick_client(nick);
    srv.clear_empty_channels();
    return;
}