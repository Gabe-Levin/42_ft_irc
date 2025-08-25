#include "../Client.hpp"
#include "../Messages.hpp"

	/*
(SOURCE LEA)
https://modern.ircdocs.horse/#invite-message
									done?
    RPL_INVITING (341)			=>	no
    ERR_NEEDMOREPARAMS (461)	=>	no
    ERR_NOSUCHCHANNEL (403)		=>	no
    ERR_NOTONCHANNEL (442)		=>	no
    ERR_CHANOPRIVSNEEDED (482)	=>	no
    ERR_USERONCHANNEL (443)		=>	no
	ERR_NOSUCHNICK (401)		=>	no

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | INVITE without <nick> or <channel>
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_CHANOPRIVSNEEDED (482)    | NO          | Only channel operators can invite
ERR_USERONCHANNEL (443)       | NO          | Target user is already in channel
ERR_NOSUCHNICK (401)          | NO          | Target nickname does not exist

*/

void Client::do_invite(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel;
    std::string nick;
    iss >> channel;
    iss >> nick;

    Channel *e_channel = (Channel::find_channel(channel, srv));
    if(e_channel != NULL)
    {
        c.outbuf += "Error\r\n";
        return;
    }

    Client *invitee = srv.get_client(nick);
    if (invitee != NULL)
    {
        c.outbuf += "Error\r\n";
        return;
    }

    (*e_channel).invite_list.push_back(invitee);
    c.outbuf += ":" + c.nick + "!" + c.nick + "@host"+ " INVITE " + invitee->nick + " :" + e_channel->_name + "\r\n";
    invitee->outbuf += ":" + c.nick + "!" + c.nick + "@host"+ " INVITE " + invitee->nick + " :" + e_channel->_name + "\r\n";
    return;
}