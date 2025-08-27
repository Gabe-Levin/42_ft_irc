#include "../Client.hpp"
#include "../Messages.hpp"

/*
(SOURCE LEA)
If <target> is a nickname that does not exist on the network,
the ERR_NOSUCHNICK (401) numeric is returned. 
If <target> is a different nick than the user who sent the command, 
the ERR_USERSDONTMATCH (502) numeric is returned.
If <modestring> is not given, the RPL_UMODEIS (221) numeric is sent back
containing the current modes of the target user.
If <modestring> is given, the supplied modes will be applied, and a MODE
message will be sent to the user containing the changed modes. If one or more
modes sent are not implemented on the server, the server MUST apply the modes
that are implemented, and then send the ERR_UMODEUNKNOWNFLAG (501) in reply 
along with the MODE message.
option we are not implementing:
 o: Give/take channel operator privilege -can only be used on Channel
 i: Invisible User Mode

Error Replies:
							done?
ERR_NOSUCHNICK (401)		=>	no
ERR_UMODEUNKNOWNFLAG (501)	=>	no
ERR_USERSDONTMATCH (502)	=>	no

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NOSUCHCHANNEL (403)       | YES          | Channel does not exist
ERR_NEEDMOREPARAMS (461)      | YES          | Missing arguments for mode (+k, +l, +o/-o)
ERR_UNKNOWNMODE (472)         | YES          | Unknown channel mode flag
ERR_CHANOPRIVSNEEDED (482)    | YES          | User is not channel operator
ERR_USERNOTINCHANNEL (441)    | YES          | Target user for +o/-o not in channel

*/

void Client::do_mode(std::istringstream &iss, Server &srv, Client &c)
{
    std::string channel_name, flag;
    iss >> channel_name;
    iss >> flag;

    if(channel_name.empty())
    {
        Msg::ERR_NEEDMOREPARAMS(srv, c, "MODE");
        return;
    }

    Channel* channel = Channel::find_channel(channel_name, srv);
    if(channel == NULL)
    {
        Msg::ERR_NOSUCHCHANNEL(srv, c, channel_name);
        return;
    }

    if(flag.empty())
    {
        Msg::RPL_CHANNELMODEIS(srv, c, *channel);
        return;
    }

    if(!channel->is_on_client_list(c.nick))
    {
        Msg::ERR_USERNOTINCHANNEL(srv, c, *channel, c.nick);
        return;
    }

    Client * op = (*channel).find_operator(c.nick);
    if(op == NULL)
    {
        Msg::ERR_CHANOPRIVSNEEDED(srv, c, *channel);
        return;
    }

    // Handle flags
    if(flag == "+i") //Make channel invite-only
    {
        channel->invite_only = true;
        Msg::MODE(srv, c, *channel, flag);
    }
    else if(flag == "-i") //Lift invite-only restriction
    {
        channel->invite_only = false;
        Msg::MODE(srv, c, *channel, flag);

    }
    else if(flag == "+t")
    {
        channel->topic_restricted = true;
        Msg::MODE(srv, c, *channel, flag);

    }
    else if(flag == "-t")
    {
        channel->topic_restricted = false;
        Msg::MODE(srv, c, *channel, flag);
    }
    else if(flag == "+k") //Set channel password
    {
        std::string secretpwd;
        iss >> secretpwd;
        if(secretpwd.empty())
        {
            Msg::ERR_NEEDMOREPARAMS(srv, c, "MODE");
            return;
        }
        else
        {
            channel->secretpwd = secretpwd;
            Msg::MODE(srv, c, *channel, flag);
        }
    }
    else if(flag == "-k") //Lift channel password
    {
        channel->secretpwd.erase();
        Msg::MODE(srv, c, *channel, flag);
    }
    else if(flag == "+o" || flag == "-o") //Make/Kick operators
    {
        std::string nick;
        iss >> nick;
        if(nick.empty())
            Msg::ERR_NEEDMOREPARAMS(srv, c, "MODE");

        else if(!channel->is_on_client_list(nick))
        {
            Msg::ERR_NOSUCHNICK(srv, c, nick);
        }

        else if(flag[0]== '+')
        {
            channel->make_operator(nick);
            Msg::MODE(srv, c, *channel, flag, nick);
        }
        else if(flag[0]== '-')
        {
            channel->kick_operator(nick);
            Msg::MODE(srv, c, *channel, flag, nick);
        }
    }
    else if(flag == "+l") //Set max_clients
    {
        std::string max_clients_input; 
        iss >> max_clients_input;

        if(max_clients_input.empty())
        {
            Msg::ERR_NEEDMOREPARAMS(srv, c, "MODE");
            return;
        }

        for (size_t i = 0; i < max_clients_input.size(); ++i)
        {
            if (!isdigit(max_clients_input[i]))
            {
                Msg::ERR_NEEDMOREPARAMS(srv, c, "MODE"); // invalid param = treat as missing
                return;
            }
        }

        int max_clients = atoi(max_clients_input.c_str());
        channel->max_clients = max_clients;
        Msg::MODE(srv, c, *channel, flag);
    }
    else if(flag == "-l") //Lift max_clients
    {
        channel->max_clients = 0;
        Msg::MODE(srv, c, *channel, flag);
    }
    else
        Msg::ERR_UNKNOWNMODE(srv, c, flag);
    return;
}