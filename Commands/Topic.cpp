#include "../Client.hpp"

/*

(SOURCE LEA)
https://modern.ircdocs.horse/#topic-message
										done?
    ERR_NEEDMOREPARAMS (461)		=>	no
    ERR_NOSUCHCHANNEL (403)			=>	no
    ERR_NOTONCHANNEL (442)			=>	no
    ERR_CHANOPRIVSNEEDED (482)		=>	no
    RPL_NOTOPIC (331)				=>	no
    RPL_TOPIC (332)					=>	no
	(??? Don't know if necessary (berni, 28.08.)
    (Sourc: Lea)RPL_TOPICWHOTIME (333)		=> NO : we dont hav history in our server !! (Source: Lea)

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | TOPIC without channel
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_CHANOPRIVSNEEDED (482)    | NO          | Channel has +t and user is not channel operator
ERR_NOTONCHANNEL (442)        | NO          | User not on channel (if you enforce membership for viewing/setting topic)


*/

void Client::do_topic(std::istringstream &iss, Server &srv, Client &c)
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
    if(op == NULL && channel->topic_restricted)
    {
        c.outbuf += "Sorry bud, you are not an operator. \r\n";
        return;
    }
    std::string topic;

    if(iss >> topic)
    {
        channel->topic = topic;
        c.outbuf += "332 " + c.nick + " " + channel->_name + " :" + topic + "\r\n";
    }
    else
    {
        if(topic.empty()) // Empty topic sent
            channel->topic.erase();

        else if(channel->topic.empty()) // No topic found
            c.outbuf += "331 " + c.nick + " " + channel->_name + " :No topic is set\r\n";

        else // Show topic
            c.outbuf += channel->topic + "\r\n";
    }
    return;
}