#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>

#include "Client.hpp"
#include "Channel.hpp"

struct Channel;
struct Client;

struct Server
{
    const char* _port;
    const char* _password;
    std::vector<Channel> channels;
    std::vector<Client*> clients;

    Server();
    Server(const char *port, const char* password);
    Client * get_client(std::string nick);
    static int set_nonblocking(int fd);
    static int create_listen_socket(const char* port);
};

#endif