#include "Channel.hpp"

// Canonical Form
Channel::Channel(): invite_only(false), topic_restricted(false), max_clients(0){}

Channel::Channel(const Channel& other)
    : invite_only(other.invite_only)
    , topic_restricted(other.topic_restricted)
    , max_clients(other.max_clients)
    , _name(other._name)
    , topic(other.topic)
    , secretpwd(other.secretpwd)
    , clients(other.clients)        
    , op_list(other.op_list)        
    , invite_list(other.invite_list)
{}

Channel& Channel::operator=(const Channel& other) 
{
    if (this != &other) {
        invite_only     = other.invite_only;
        topic_restricted= other.topic_restricted;
        max_clients     = other.max_clients;
        _name           = other._name;
        topic           = other.topic;
        secretpwd       = other.secretpwd;
        clients         = other.clients;     
        op_list         = other.op_list;     
        invite_list     = other.invite_list; 
    }
    return *this;
}

Channel::~Channel(){}

Channel::Channel(std::string name):  invite_only(false), topic_restricted(false), max_clients(0), _name(name){};


// Methods
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

void Channel::broadcast_to_others(std::string msg, std::string nick)
{
    for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if((*it)->nick != nick)
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
