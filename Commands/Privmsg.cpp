#include "../Client.hpp"

/*
    https://modern.ircdocs.horse/#privmsg-message

ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NORECIPIENT (411)         | NO          | PRIVMSG without recipient
ERR_NOTEXTTOSEND (412)        | NO          | PRIVMSG without message
ERR_NOSUCHNICK (401)          | NO          | Target nickname does not exist
ERR_NOSUCHCHANNEL (403)       | NO          | Target channel does not exist - !! Lea wasn"t going for this one
ERR_CANNOTSENDTOCHAN (404)    | NO          | Cannot send to channel (e.g. moderated +m)


	(??? Don't know if necessary (berni, 28.08.)
	(Sourc: Lea)RPL_AWAY = 301 // not doing that one anymore, doesn't make sense, as we don't register users

*/

void Client::do_privmsg(std::istringstream &iss, Server &srv, Client &c)
{
    std::string target;
    std::string message;
    iss >> target;

    std::getline(iss, message);
    if (!message.empty() && message[0] == ':')
        message.erase(0, 1);
    
    // Handle messages to a channel
    if(!target.empty() && target[0] == '#')
    { 
        Channel *existing_channel = Channel::find_channel(target, srv);
        if (existing_channel == NULL)
        {
            c.outbuf += "Target channel '" + target + "' does not exist. \r\n";
            return;
        }

        if (!(*existing_channel).is_on_client_list(c.nick))
        {
            c.outbuf += "You are not member of this channel: " + target + ". \r\n";
            return;
        }

        for(std::vector<Client*>::iterator it = existing_channel->clients.begin(); it != existing_channel->clients.end(); ++it)
        {
            if(c.nick != (*it)->nick) //:Alice!alice@127.0.0.1 PRIVMSG #42network :hello everyone!
            {
                (*it)->outbuf += ":" + c.nick + " " + "PRIVMSG" + " " + target + message + "\r\n";
            }
        }
    }
    // Handle direct messages to another client
    else
    {
        for(std::vector<Client*>::iterator it = srv.clients.begin(); it != srv.clients.end(); ++it)
        {
            if(target == (*it)->nick)
            {
                (*it)->outbuf += ":" + c.nick + " " + "PRIVMSG" + " " + target + message + "\r\n";
            }
        }
    }
    return;
}