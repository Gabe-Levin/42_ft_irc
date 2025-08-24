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
    std::string _name;
    const char* _port;
    const char* _password;
    std::string _created;
    std::vector<Channel> channels;
    std::vector<Client*> clients;

    Server(){_name = get_server_name();};
    Server(const char *port, const char* password);
    Client * get_client(std::string nick);
    static int set_nonblocking(int fd);
    static int create_listen_socket(const char* port);
    std::string get_date_str();
    bool is_nick_taken(std::string nick);


    private:
        std::string get_server_name();

};

#endif