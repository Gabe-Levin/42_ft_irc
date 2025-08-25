#include "Channel.hpp"

Channel* Channel::find_channel(std::string channel, Server &srv)
{
    for(std::vector<Channel>::iterator it = srv.channels.begin(); it != srv.channels.end(); it++)
    {
        if(it->_name == channel)
            return &(*it);
    }
    return NULL;
}

Client* Channel::find_operator(std::string nick)
{
    for(std::vector<Client*>::iterator it = op_list.begin(); it != op_list.end(); it++)
    {
        if((*it)->nick == nick)
            return *it;
    }
    return NULL;
}

bool Channel::kick_client(std::string nick)
{
    for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if(*it && nick == (*it)->nick)
        {
            clients.erase(it);
            kick_operator(nick);
            return true;
        }
    }
    return false;
}

bool Channel::kick_operator(std::string nick)
{
    for(std::vector<Client*>::iterator it = op_list.begin(); it != op_list.end(); ++it)
    {
        if(nick == (*it)->nick)
        {
            op_list.erase(it);
            return true;
        }
    }
    return false;
}

bool Channel::is_on_client_list(std::string nick)
{
    for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if(nick == (*it)->nick)
        {
            return true;
        }
    }
    return false;
}

void Channel::broadcast(std::string msg)
{
    for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        (*it)->outbuf += msg;
    }
}

bool Channel::is_on_invite_list(std::string nick)
{
    for(std::vector<Client*>::iterator it = invite_list.begin(); it != invite_list.end(); ++it)
    {
        if(nick == (*it)->nick)
        {
            return true;
        }
    }
    return false;
}

bool Channel::make_operator(std::string nick)
{
    if (find_operator(nick) != NULL)
        return false;
    for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if(nick == (*it)->nick)
        {
            op_list.push_back(*it);
            return true;
        }
    }
    return false;
}
bool Channel::is_operator(std::string nick)
{
    for(std::vector<Client*>::iterator it = op_list.begin(); it != op_list.end(); it++)
    {
        if((*it)->nick == nick)
            return true;
    }
    return false;
}
