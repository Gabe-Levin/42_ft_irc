#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"
#include "Server.hpp"

struct Client;
struct Server;

struct Channel
{
    bool invite_only;
    bool topic_restricted;
    size_t max_clients;
    std::string _name;
    std::string topic;
    std::string secretpwd;
    std::vector<Client*> clients;
    std::vector<Client*> op_list;
    std::vector<Client*> invite_list;
    
    Channel(){};
    Channel(std::string name):  invite_only(false), topic_restricted(false), max_clients(0), _name(name){};

    bool is_on_invite_list(std::string nick);
    bool rm_from_invite_list(std::string nick);
    bool is_on_client_list(std::string nick);
    bool is_channel_joinable(std::string nick);
    bool make_operator(std::string nick);
    bool kick_client(std::string nick);
    bool kick_operator(std::string nick);
    Client* find_operator(std::string nick);
    void broadcast(std::string msg);

    static Channel* find_channel(std::string channel, Server &srv);
};

#endif