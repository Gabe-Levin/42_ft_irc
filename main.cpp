#include <iostream>
#include <poll.h>
#include <vector>
#include <map>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>

#include "Client.hpp"
#include "Server.hpp"

void handle_cmd(Client &c, const std::string &line, const std::string &server_pass)
{
    std::istringstream iss(line);
    std::string cmd;

    iss >> cmd; //set the first word in the iss stream to cmd

    if(cmd == "PASS")
    {
        std::string pass;
        iss >> pass;
        if(pass == server_pass)
        {
            c.auth = true;
            return;
        }
        else
        {
            c.outbuf += "Wrong password. Reseting buffer.\r\n";
            c.toDisconnect = true;
            return;
        }
        
    }
    else if(cmd == "NICK")
    {
        iss >> c.nick;
    }
    else if (cmd == "USER")
    {
        iss >> c.user;
    }
}

int main(int argc, char** argv)
{
    if(argc!= 3)
    {
        std::cerr << "Usage : ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    Server srv(argv[1], argv[2]);

    int listenfd = Server::create_listen_socket(srv.port);
    if(listenfd < 0) return 1;

    std::vector<struct pollfd> pfds;
    pfds.push_back(pollfd());
    pfds[0].fd = listenfd;
    pfds[0].events = POLLIN;

    std::map<int, Client> clients;
    
    std::cout << "Echo server listening on port " << srv.port << " ...\n";

    while(true)
    {
        // Update events based on the outbuf
        for(size_t i = 0; i < pfds.size(); ++i)
        {
            int fd = pfds[i].fd;
            std::map<int, Client>::iterator it;
            it = clients.find(fd);

            if(it != clients.end())
            {
                pfds[i].events = POLLIN;
                if(!it->second.outbuf.empty())
                    pfds[i].events |= POLLOUT;
            }
        }

        // Monitor the fds in pfds
        int ready = poll(&pfds[0], pfds.size(), -1);
        if(ready < 0)
        {
            if(errno == EINTR) continue;
            std::cerr << "poll error: " << std::strerror(errno) << std::endl; 
        }

        // Accept new clients
        Client::accept_new(pfds, clients, listenfd);

        // 2) IO for clients

        for(size_t i = 0; i < pfds.size();)
        {
            int fd = pfds[i].fd;
            std::map<int, Client>::iterator it = clients.find(fd);
            if(it == clients.end())
            {
                i++;
                continue;
            }

            bool erased = false;

            if (pfds[i].revents & POLLIN)
            {
                char buf[512];
                while(true)
                {
                    int n = recv(fd, buf, sizeof(buf), 0);
                    if(n > 0)
                    {
                        it->second.inbuf.append(buf, n);
                        std::string line;
                        while(Client::pop_line(it->second.inbuf, line))
                        {
                            handle_cmd(it->second, line, srv.password);
                            // std::string reply = ":echo NOTICE * :" + line + "\r\n";
                            // it->second.outbuf += reply;
                        }
                    }
                    else if (n == 0)
                    {
                        // Peer closed. ie. nothing to read
                        Client::close_client(pfds, clients, i);
                        erased = true;
                        break;
                    }
                    else
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        // Real error
                        Client::close_client(pfds, clients, i);
                        erased = true;
                        break;
                    }
                }
            }
            if (erased) continue;

            // Writeable?
            if((pfds[i].revents & POLLOUT) && !it->second.outbuf.empty())
            {
                const char* data = it->second.outbuf.data();
                size_t leftover = it->second.outbuf.size();
                int n = send(fd, data, leftover, 0);
                if (n > 0)
                {
                    it -> second.outbuf.erase(0, n);

                    // Close client if outbuf is empty and toDisconnect is true
                    if (it->second.outbuf.empty() && it->second.toDisconnect)
                    {
                        Client::close_client(pfds, clients, i);
                        erased = true;
                    }
                }
                else if (n < 0)
                {
                    if(!(errno == EAGAIN || errno == EWOULDBLOCK)) 
                    {
                        Client::close_client(pfds, clients ,i);
                        erased = true;
                    }
                }
            }
            if (!erased) ++i;
        }
        // Server::disconnect_sockets(clients, pfds);
    }

    close(listenfd);
    return 0;
}