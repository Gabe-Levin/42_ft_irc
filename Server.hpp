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

struct Server
{
    const char* port;
    const char* password;
    std::vector<struct Channel> channels;

    Server();
    Server(const char *port, const char* password);
    static int set_nonblocking(int fd);
    static int create_listen_socket(const char* port);
};


#endif