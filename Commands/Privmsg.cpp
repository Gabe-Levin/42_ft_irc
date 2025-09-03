#include "../Client.hpp"
#include "../Messages.hpp"

void Client::do_privmsg(std::istringstream &iss, Server &srv, Client &c)
{
    std::string target;
    std::string message;
    iss >> target;

    if(target.empty())
    {
        Msg::ERR_NORECIPIENT(srv, c, "PRIVMSG");
        return;
    }

    iss >> message;
    if (!message.empty() && message[0] == ':')
    {
        message.erase(0, 1);
        std::string rest;
        std::getline(iss, rest);
        message += rest;
    }

    if (message.empty())
    {
        Msg::ERR_NOTEXTTOSEND(srv, c);
        return;
    }
    
    // Handle messages to a channel
    if(target[0] == '#')
    { 
        Channel *channel = Channel::find_channel(target, srv);

        if (channel == NULL)
        {
            Msg::ERR_NOSUCHCHANNEL(srv, c, target);
        }

        else if (!(*channel).is_on_client_list(c.nick))
        {
            Msg::ERR_CANNOTSENDTOCHAN(srv, c, *channel);
        }

        else 
            Msg::CHANNEL(srv, c, *channel, message);
        return;
    }

    // Handle direct messages to another client
    Client *target_c = srv.get_client(target);
    if(!srv.is_client(target) || !target_c->registered)
        Msg::ERR_NOSUCHNICK(srv, c, target);
    else 
    {
        if(message[0])
        Msg::CLIENT(srv, c, target, message);
    }
    return;
}