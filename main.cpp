#include <iostream>
#include <poll.h>
#include <vector>
#include <map>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

struct Client
{
    int fd;
    std::string inbuf;
    std::string outbuf;
    Client(): fd(-1){};
    Client(int f): fd(f) {};
};

int main(int argc, char** argv)
{
    if(argc!= 3)
    {
        std::cerr << "Usage : ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    const char* port = argv[1];

    int listenfd = create_listen_socket(port);
    if(listenfd < 0) return 1;

    std::vector<struct pollfd> pfds;
    pfds.push_back(pollfd());
    pfds[0].fd = listenfd;
    pfds[0].events = POLLIN;

    std::map<int, Client> clients;
    
    std::cout << "Echo server listening on port " << port << " ...\n";

    while(true)
    {
        // Update events based on the outbuf
        for(int i = 0; i < pfds.size(); ++i)
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
        if(pfds[0].revents & POLLIN)
        {
            while(true)
            {
                struct sockaddr_storage ss;
                socklen_t slen = sizeof(ss);
                int cfd = accept(listenfd, (struct sockaddr*)&ss, &slen);
                if(cfd < 0)
                {
                    if(errno == EAGAIN || errno == EWOULDBLOCK) break;
                    std::cerr << "accept error: " << std::strerror(errno) << std::endl;
                    break;
                }
                if(set_nonblocking(cfd) < 0)
                {
                    std::cerr << "fcntl nonblock (client) failed\n";
                    close(cfd);
                    continue;
                }

                Client cli(cfd);
                clients[cfd] = cli;

                struct pollfd p;
                std::memset(&p, 0, sizeof(p));
                p.fd = cfd;
                p.events = POLLIN;
                pfds.push_back(p);

                // Greeting
                clients[cfd].outbuf += ":echo 001 * :Welcome to the tiny echo server\r\n";
            }
        }

        // 2) IO for clients

        for(int i = 0; i < pfds.size();)
        {
            int fd = pfds[i].fd;
            std::map<int, Client>::iterator it = clients.find(fd);
            if(it == clients.end())
            {
                i++;
                continue;
            }
        }



    }

}