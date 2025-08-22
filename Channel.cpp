#include "Channel.hpp"

// void Channel::join(Client client)
// {
//     clients.push_back(client);
// }

Channel* Channel::find_channel(std::string channel, Server &srv)
{
    for(std::vector<Channel>::iterator it = srv.channels.begin(); it != srv.channels.end(); it++)
    {
        if(it->_name == channel)
            return &(*it);
    }
    return NULL;
}

Client* Channel::find_operator(std::string nick, Channel &channel)
{
    for(std::vector<Client*>::iterator it = channel.op_list.begin(); it != channel.op_list.end(); it++)
    {
        if((*it)->nick == nick)
            return *it;
    }
    return NULL;
}

bool Channel::kick_client(std::string nick, Channel &channel)
{
    for(std::vector<Client*>::iterator it = channel.clients.begin(); it != channel.clients.end(); ++it)
    {
        if(*it && nick == (*it)->nick)
        {
            channel.clients.erase(it);
            Channel::kick_operator(nick, channel);
            return true;
        }
    }
    return false;
}

bool Channel::kick_operator(std::string nick, Channel &channel)
{
    for(std::vector<Client*>::iterator it = channel.op_list.begin(); it != channel.op_list.end(); ++it)
    {
        if(nick == (*it)->nick)
        {
            channel.op_list.erase(it);
            return true;
        }
    }
    return false;
}

bool Channel::find_client(std::string nick, Channel &channel)
{
    for(std::vector<Client*>::iterator it = channel.clients.begin(); it != channel.clients.end(); ++it)
    {
        if(nick == (*it)->nick)
        {
            return true;
        }
    }
    return false;
}
