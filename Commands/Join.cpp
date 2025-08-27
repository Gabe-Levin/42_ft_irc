#include "../Client.hpp"
#include "../Messages.hpp"

/*
(SOURCE LEA)
    https://modern.ircdocs.horse/#join-message
									done?
    ERR_NOTREGISTERED (451)		=>	YES
	ERR_NEEDMOREPARAMS (461)	=>	no
    ERR_NOSUCHCHANNEL (403) 	=>	no
    ERR_TOOMANYCHANNELS (405)	=> (LEA: ) we ignore that...
    ERR_BADCHANNELKEY (475)		=>	no
    x ERR_BANNEDFROMCHAN (474)	=> (LEA: )we don't ban
    ERR_CHANNELISFULL (471)		=> (LEA: )TODO: we ignore that... our limit is MAX_INT ? ==> nop
    ERR_INVITEONLYCHAN (473)	=>	no
    ERR_BADCHANMASK (476)		=>	(LEA: )???
    RPL_TOPIC (332)				=>	no
    RPL_TOPICWHOTIME (333)		=>	(LEA: )we don't do that (no history in our modest server)
    RPL_NAMREPLY (353)			=>	no
    RPL_ENDOFNAMES (366)		=>	no

(SOURCE CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | YES          | JOIN without channel
ERR_BADCHANMASK (476)         | YES          | Invalid channel name
ERR_INVITEONLYCHAN (473)      | YES          | JOIN to +i channel without invite
ERR_BADCHANNELKEY (475)       | YES          | JOIN with wrong key on +k channel
ERR_CHANNELISFULL (471)       | YES          | JOIN when +l channel limit is reached

RPL_TOPIC (332)               | YES         | If the channel has a topic set
RPL_NOTOPIC (331)             | YES         | If no topic is set on the channel
RPL_NAMREPLY (353)            | YES         | List of visible nicks in channel (@ prefix for ops)
RPL_ENDOFNAMES (366)          | YES         | End of NAMES list marker
*/

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
    std::string remainder;
    iss >> channel_name;
    
    

    if(channel_name.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "JOIN");
        return;
    }

    if(iss >> remainder)
        channel_name += remainder;

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