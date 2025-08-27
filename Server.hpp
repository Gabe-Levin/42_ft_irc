#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <memory>

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
    std::list<Client> clients;


    Server(){_name = get_server_name();};
    Server(const char *port, const char* password);
    Server(const Server& other);
    Server& operator=(const Server& other); 
    ~Server();


    Client * get_client(std::string nick);
    Client * get_client(int fd);
    Channel * find_channel(std::string name);
    bool is_client(std::string nick);
    bool rm_client(int fd);
    bool is_nick_taken(std::string nick);
    std::string get_date_str(); 
    void accept_new(std::vector<struct pollfd> &pfds, int listenfd);
    int setup_listen_socket(std::vector<struct pollfd> &pfds);
    void set_poll_policy(std::vector<struct pollfd> &pfds);
    bool handle_client_input(Server &srv, Client &client, int &fd);
    bool handle_client_output(Client &client, int &fd);
    void clear_empty_channels();

    static int set_nonblocking(int fd);
    static bool is_valid_input(int argc, char** argv);

    private:
        std::string get_server_name();
        int create_listen_socket();

};

#endif