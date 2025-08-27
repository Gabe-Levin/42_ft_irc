#include "../Client.hpp"
#include "../Messages.hpp"


void Client::do_part(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel_name;
    iss >> channel_name;

    if(channel_name.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "PART");
    }

    Channel* channel = Channel::find_channel(channel_name, srv);
    if(channel == NULL)
    {
        Msg::ERR_NOSUCHCHANNEL(srv, c, channel_name);
        return;
    }

    if(!channel->is_on_client_list(c.nick))
    {
        Msg::ERR_USERNOTINCHANNEL(srv, c, *channel, c.nick);
        return;
    }

    Msg::PART(srv, c , *channel);
    channel->kick_client(c.nick);
    srv.clear_empty_channels();
}

