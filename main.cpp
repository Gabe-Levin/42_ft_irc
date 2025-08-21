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
#include "Channel.hpp"

int main(int argc, char** argv)
{
    if(argc!= 3)
    {
        std::cerr << "Usage : ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    Server srv(argv[1], argv[2]);
    std::map<int, Client> clients;

    // Setup listen socket
    int listenfd = Server::create_listen_socket(srv.port);
    if(listenfd < 0) return 1;

    std::vector<struct pollfd> pfds;
    pfds.push_back(pollfd());
    pfds[0].fd = listenfd;
    pfds[0].events = POLLIN;

    std::cout << "IRC server listening on port " << srv.port << " ...\n";

    // Main server loop
    while(true)
    {
        // Define the poll policy
        for(size_t i = 0; i < pfds.size(); ++i)
        {
            int fd = pfds[i].fd;
            std::map<int, Client>::iterator it;
            it = clients.find(fd);

            if(it != clients.end())
            {
                pfds[i].events = POLLIN; // always read
                if(!it->second.outbuf.empty())
                    pfds[i].events |= POLLOUT; // write only if msg pending
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

        // IO for clients
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

            // Handle new message from client
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
                            Client::handle_cmd(it->second, line, srv);
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

            // Handle sending messages to clients
            if((pfds[i].revents & POLLOUT) && !it->second.outbuf.empty())
            {
                const char* data = it->second.outbuf.data();
                size_t leftover = it->second.outbuf.size();
                int n = send(fd, data, leftover, 0);
                if (n > 0)
                {
                    it -> second.outbuf.erase(0, n);

                    // Close client check
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
    }

    close(listenfd);
    return 0;
}