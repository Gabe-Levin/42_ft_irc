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