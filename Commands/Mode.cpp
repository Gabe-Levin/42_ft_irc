#include "../Client.hpp"

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
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_NEEDMOREPARAMS (461)      | NO          | Missing arguments for mode (+k, +l, +o/-o)
ERR_UNKNOWNMODE (472)         | NO          | Unknown channel mode flag
ERR_CHANOPRIVSNEEDED (482)    | NO          | User is not channel operator
ERR_USERNOTINCHANNEL (441)    | NO          | Target user for +o/-o not in channel

*/

void Client::do_mode(std::istringstream &iss, Server &srv, Client &c)
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
    if(op == NULL)
    {
        c.outbuf += "Sorry bud, you are not an operator. \r\n";
        return;
    }

    std::string flag;
    std::string msg;
    iss >> flag;

    if(flag.empty()) {
        c.outbuf += ":server 461 " + c.nick + " MODE :Not enough parameters\r\n";
        return;
    }
    
    // TODO: get actual host ip from socket after accept
    msg = ": " + c.nick + "!" + c.user + "@host" + " MODE " + channel->_name + " " + flag + "\r\n";
    if(flag == "+i") //Make channel invite-only
    {
        channel->invite_only = true;
        channel->broadcast(msg);
    }
    else if(flag == "-i") //Lift invite-only restriction
    {
        channel->invite_only = false;
        channel->broadcast(msg);
    }
    else if(flag == "+t")
    {
        channel->topic_restricted = true;
        channel->broadcast(msg);
    }
    else if(flag == "-t")
    {
        channel->topic_restricted = false;
        channel->broadcast(msg);
    }
    else if(flag == "+k") //Set channel password
    {
        std::string secretpwd;
        iss >> secretpwd;
        if(secretpwd.empty())
            c.outbuf += ":server 461 " + c.nick + " MODE :Not enough parameters\r\n";
        else
        {
            channel->secretpwd = secretpwd;
            channel->broadcast(msg);
        }
    }
    else if(flag == "-k") //Lift channel password
    {
        channel->secretpwd.erase();
        channel->broadcast(msg);
    }
    else if(flag == "+o" || flag == "-o") //Make/Kick operators
    {
        std::string flags; 
        iss >> flags;  
        if (flags.size() != 2 || (flags[0] != '+' && flags[0] != '-') || flags[1] != 'o') {
            c.outbuf += ":server 501 " + c.nick + " :Unknown MODE flag\r\n"; // ERR_UMODEUNKNOWNFLAG
            return;
        }

        std::string nick;
        iss >> nick;
        if(nick.empty())
        {
            c.outbuf += ":server 461 " + c.nick + " MODE :Not enough parameters\r\n";
            return;
        }
        else if(flag[0]== '+')
        {
            channel->make_operator(nick);
            channel->broadcast(msg);
        }
        else if(flag[0]== '-')
        {
            channel->kick_operator(nick);
            channel->broadcast(msg);
        }
    }

    else if(flag == "+l") //Set max_clients
    {
        std::string max_clients_input; 
        iss >> max_clients_input;

        int max_clients = atoi(max_clients_input.c_str());
        channel->max_clients = max_clients;
        channel->broadcast(msg);
    }
    else if(flag == "-l") //Lift max_clients
    {
        channel->max_clients = 0;
        channel->broadcast(msg);
    }
}