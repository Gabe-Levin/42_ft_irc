#include "../Client.hpp"
#include "../Messages.hpp"

	/*
(SOURCE LEA)
https://modern.ircdocs.horse/#invite-message
									done?
    RPL_INVITING (341)			=>	YES
    ERR_NEEDMOREPARAMS (461)	=>	YES
    ERR_NOSUCHCHANNEL (403)		=>	YES
    ERR_NOTONCHANNEL (442)		=>	YES
    ERR_CHANOPRIVSNEEDED (482)	=>	YES
    ERR_USERONCHANNEL (443)		=>	YES
	ERR_NOSUCHNICK (401)		=>	YES

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | YES          | INVITE without <nick> or <channel>
ERR_NOSUCHCHANNEL (403)       | YES          | Channel does not exist
ERR_CHANOPRIVSNEEDED (482)    | YES          | Only channel operators can invite
ERR_USERONCHANNEL (443)       | YES          | Target user is already in channel
ERR_NOSUCHNICK (401)          | YES          | Target nickname does not exist

*/

void Client::do_invite(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel_name;
    std::string nick;
    iss >> channel_name;
    iss >> nick;

    if(channel_name.empty() || nick.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "INVITE");
        return;
    }

    Channel *channel = (Channel::find_channel(channel_name, srv));
    if(channel == NULL)
    {
        Msg::ERR_NOSUCHCHANNEL(srv, c, channel_name);
        return;
    }

    Client *invitee = srv.get_client(nick);
    if (invitee == NULL)
    {
        Msg::ERR_NOSUCHNICK(srv, c, nick);
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