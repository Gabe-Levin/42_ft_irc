#include "../Client.hpp"
#include "../Messages.hpp"

bool isValidChannelName(const std::string& name) {
    if (name.empty())
        return false;
    if (name[0] != '#' && name[0] != '&')
        return false;
    if (name.size() == 1)
        return false;
    for (size_t i = 0; i < name.size(); ++i) {
        char c = name[i];
        if (c == ' ' || c == ',' || c == '\a' || c == '\r' || c == '\n')
            return false;
    }
    return true;
}

void Client::do_join(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel_name;
    std::string password;
    iss >> channel_name;
    
    if(channel_name.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "JOIN");
        return;
    }

    
    if(!isValidChannelName(channel_name))
    {
        Msg::ERR_BADCHANMASK(srv, c, channel_name);
        return;
    }
    
    Channel *channel = srv.find_channel(channel_name);
    if(channel == NULL) // New channel
    {
        Channel new_channel(channel_name);
        srv.channels.push_back(new_channel);
        channel = &srv.channels.back(); // Pointer to the server owned channel
        (*channel).clients.push_back(&c);
        (*channel).op_list.push_back(&c);
        
        Msg::BROADCAST_JOIN(srv, c, *channel);
        Msg::RPL_NOTOPIC(srv, c, *channel);
        Msg::RPL_NAMREPLY(srv, c, *channel);
        Msg::RPL_ENDOFNAMES(srv, c, *channel);
        return;
    }
    if ((*channel).is_on_client_list(c.nick))
    {
        Msg::ERR_USERONCHANNEL(srv, c, *channel);
        return;
    }
    
    // Check if existing channel has restrictions
    if((*channel).invite_only && !(*channel).is_on_invite_list(c.nick))
    {
        Msg::ERR_INVITEONLYCHAN(srv, c, channel->_name);
        return;
    }
    
    if(!(*channel).secretpwd.empty())
    {
        std::string secretpwd;
        iss >> secretpwd;

        if((*channel).secretpwd != secretpwd)
        {
            Msg::ERR_BADCHANNELKEY(srv, c, channel->_name);
            return;
        }
    }

    if((*channel).max_clients != 0 && (*channel).max_clients < ((*channel).clients.size() +1))
    {
        Msg::ERR_CHANNELISFULL(srv, c, (*channel)._name);
        return;
    }

    (*channel).clients.push_back(&c);

    // Messaging for Joining and existing channel
    Msg::BROADCAST_JOIN(srv, c, *channel);
    if((*channel).topic.empty())
        Msg::RPL_NOTOPIC(srv, c, *channel);
    else
        Msg::RPL_TOPIC(srv, c, *channel);
    Msg::RPL_NAMREPLY(srv, c, *channel);
    Msg::RPL_ENDOFNAMES(srv, c, *channel);
    return;
}