#include "../Client.hpp"
#include "../Messages.hpp"

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