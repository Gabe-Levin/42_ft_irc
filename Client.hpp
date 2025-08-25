#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <poll.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <sys/socket.h>
#include <sstream>
#include <string>
#include <cstdlib> 

#include "Server.hpp"
struct Server;

struct Client
{
    int fd;
    bool password;
    bool registered;
    bool toDisconnect;
    std::string inbuf;
    std::string outbuf;
    std::string nick;
    std::string user;
    std::string realname;

    Client(): fd(-1), password(false), registered(false), toDisconnect(false){};
    Client(int f): fd(f) {};
    static void close_client(std::vector<struct pollfd>& pfds, std::map<int, Client>& clients, size_t idx);
    static bool pop_line(std::string &buffer, std::string &line);
    static void accept_new(std::vector<struct pollfd> &pfds, std::map<int, Client> &clients, Server &srv, int listenfd);
    static void handle_cmd(Client &c, const std::string &line, Server &srv);

    //Cmds
    void do_invite(std::istringstream &iss, Server &srv, Client &c);
    void do_join(std::istringstream &iss, Server &srv, Client &c);
    void do_kick(std::istringstream &iss, Server &srv, Client &c);
    void do_mode(std::istringstream &iss, Server &srv, Client &c);
    void do_nick(std::istringstream &iss, Server &srv, Client &c);
    void do_pass(std::istringstream &iss, Server &srv, Client &c);
    void do_topic(std::istringstream &iss, Server &srv, Client &c);
    void do_privmsg(std::istringstream &iss, Server &srv, Client &c);
    void do_user(std::istringstream &iss, Server &srv, Client &c);
};

#endif