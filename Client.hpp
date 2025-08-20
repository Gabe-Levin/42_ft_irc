#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <poll.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <sys/socket.h>


struct Client
{
    int fd;
    bool auth;
    bool toDisconnect;
    std::string inbuf;
    std::string outbuf;
    std::string nick;
    std::string user;

    Client(): fd(-1), auth(false), toDisconnect(false){};
    Client(int f): fd(f) {};
    static void close_client(std::vector<struct pollfd>& pfds, std::map<int, Client>& clients, size_t idx);
    static bool pop_line(std::string &buffer, std::string &line);
    static void accept_new(std::vector<struct pollfd> &pfds, std::map<int, Client> &clients, int listenfd);
};


#endif