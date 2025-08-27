#include "../Client.hpp"
#include "../Messages.hpp"

void Client::do_names(std::istringstream &iss, Server &srv, Client &c)
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

    Msg::RPL_NAMREPLY(srv, c, *channel);
    Msg::RPL_ENDOFNAMES(srv, c, *channel);
}

