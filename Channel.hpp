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
    std::string _name;
    std::string topic;
    std::vector<Client*> clients;
    std::vector<Client*> op_list;
    
    Channel(){};
    Channel(std::string name): _name(name) {};

    // void join(Client client);
    static bool find_client(std::string nick, Channel &channel);
    static Channel* find_channel(std::string channel, Server &srv);
    static Client* find_operator(std::string nick, Channel &channel);
    static bool kick_client(std::string nick, Channel &channel);
    static bool kick_operator(std::string nick, Channel &channel);
};

#endif