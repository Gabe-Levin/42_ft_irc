#include "../Client.hpp"

/*
(SOURCE LEA)
    https://modern.ircdocs.horse/#join-message
									done?
    ERR_NOTREGISTERED (451)		=>	no
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
ERR_NEEDMOREPARAMS (461)      | NO          | JOIN without channel
ERR_BADCHANMASK (476)         | NO          | Invalid channel name
ERR_INVITEONLYCHAN (473)      | NO          | JOIN to +i channel without invite
ERR_BADCHANNELKEY (475)       | NO          | JOIN with wrong key on +k channel
ERR_CHANNELISFULL (471)       | NO          | JOIN when +l channel limit is reached


*/

void Client::do_join(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel;
    if(!c.registered)
        c.outbuf += "You must register before you can join/create a channel \r\n";
    else
    {
        iss >> channel; //TODO: validate channel input
        Channel *existing_channel = Channel::find_channel(channel, srv);
        if(existing_channel == NULL)
        {
            Channel new_channel(channel);
            srv.channels.push_back(new_channel);
            srv.channels.back().clients.push_back(&c);

            c.outbuf += "Congrats, you have just created and joined a new channel: '" + channel + "'!\r\n";
            srv.channels.back().op_list.push_back(&c);
            c.outbuf += "You are now an operator on '" + channel + "'!\r\n";
            return;
        }
        if(!(*existing_channel).is_channel_joinable(c.nick))
        {
            c.outbuf += ":server 473 " + c.nick + " " + existing_channel->_name + " :Cannot join channel (+i)\r\n";
            return;
        }
        if(!(*existing_channel).secretpwd.empty())
        {
            std::string secretpwd;
            iss >> secretpwd;

            if((*existing_channel).secretpwd != secretpwd)
            {
                c.outbuf += ":server 475 " + c.nick + " " + existing_channel->_name + " :Cannot join channel (+k)\r\n";
                return;
            }
        }
        if((*existing_channel).max_clients != 0 && (*existing_channel).max_clients < ((*existing_channel).clients.size() +1))
        {
            c.outbuf += ":server 473 " + c.nick + " " + existing_channel->_name + " :Cannot join channel (+i)\r\n";
            return;
        }

        (*existing_channel).clients.push_back(&c);
        c.outbuf += "Congrats, you have just joined a new channel '" + channel + "'!\r\n";
        return;
    }
}