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
    std::vector<Client*> clients;

    Channel(){};
    Channel(std::string name): _name(name), clients(0) {};

    // void join(Client client);
    static Channel* find_channel(std::string channel, Server &srv);
};

#endif