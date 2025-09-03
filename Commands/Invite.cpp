#include "../Client.hpp"
#include "../Messages.hpp"

void Client::do_invite(std::istringstream &iss, Server &srv, Client &c)
{
    std::string nick;
    std::string channel_name;
    iss >> nick;
    iss >> channel_name; 

    if(channel_name.empty() || nick.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "INVITE");
        return;
    }

    Client *invitee = srv.get_client(nick);
    if (invitee == NULL)
    {
        Msg::ERR_NOSUCHNICK(srv, c, nick);
        return;
    }

    Channel *channel = (Channel::find_channel(channel_name, srv));
    if(channel == NULL)
    {
        Msg::ERR_NOSUCHCHANNEL(srv, c, channel_name);
        return;
    }

    if (!channel->is_on_client_list(c.nick))
    {
        Msg::ERR_NOTONCHANNEL(srv, c, *channel);
        return;
    }

    if (!channel->is_operator(c.nick))
    {
        Msg::ERR_CHANOPRIVSNEEDED(srv, c, *channel);
        return;
    }

    if(channel->is_on_client_list(nick))
    {
        Msg::ERR_USERONCHANNEL(srv, c, *channel, nick);
        return;
    }

    (*channel).invite_list.push_back(invitee);
    Msg::RPL_INVITING(srv, c, *channel, nick);
    Msg::INVITE(srv, c, *channel, *invitee);
    return;
}