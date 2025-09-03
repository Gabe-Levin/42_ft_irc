#include "../Client.hpp"
#include "../Messages.hpp"

void Client::do_topic(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel_name;
    iss >> channel_name;

    if(channel_name.empty() || (channel_name[0] == ':' && channel_name.length()==1))
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "TOPIC");
        return;
    }

    Channel* channel = Channel::find_channel(channel_name, srv);
    if(channel == NULL)
    {
        Msg::ERR_NOSUCHCHANNEL(srv, c, channel_name);   
        return;
    }
    
    if(!channel->is_on_client_list(c.nick))
    {
        Msg::ERR_NOTONCHANNEL(srv, c, *channel);
        return;
    }

    std::string topic;
    std::getline(iss, topic);
    if(topic.empty())
    {
        if(channel->topic.empty())
            Msg::RPL_NOTOPIC(srv, c, *channel);
        else
            Msg::RPL_TOPIC(srv, c, *channel);
        return;
    }

    if(channel->topic_restricted && !channel->is_operator(c.nick))
    {
        Msg::ERR_CHANOPRIVSNEEDED(srv, c, *channel);
        return;
    }

    if(channel_name[0] == ':' && channel_name.length()==1)
    {
        channel->topic.erase();
        Msg::BROADCAST_TOPIC(srv, c, *channel);
        return;
    }

    if(!topic.empty())
    {
        channel->topic = topic;
        Msg::RPL_TOPIC(srv, c, *channel);
        return;
    }

    return;
}