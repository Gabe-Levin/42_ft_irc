#include "../Client.hpp"
#include "../Messages.hpp"

/*

(SOURCE LEA)
https://modern.ircdocs.horse/#topic-message
										done?
    ERR_NEEDMOREPARAMS (461)		=>	YES
    ERR_NOSUCHCHANNEL (403)			=>	YES
    ERR_NOTONCHANNEL (442)			=>	YES
    ERR_CHANOPRIVSNEEDED (482)		=>	YES
    RPL_NOTOPIC (331)				=>	YES
    RPL_TOPIC (332)					=>	YES
	(??? Don't know if necessary (berni, 28.08.)
    (Sourc: Lea)RPL_TOPICWHOTIME (333)		=> NO : we dont hav history in our server !! (Source: Lea)

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | YES          | TOPIC without channel
ERR_NOSUCHCHANNEL (403)       | YES          | Channel does not exist
ERR_CHANOPRIVSNEEDED (482)    | YES          | Channel has +t and user is not channel operator
ERR_NOTONCHANNEL (442)        | YES          | User not on channel (if you enforce membership for viewing/setting topic)


*/

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