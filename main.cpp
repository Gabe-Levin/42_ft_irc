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
    //Input validation
    if(!Server::is_valid_input(argc, argv))
        return 1;

    //Initialize server and pollfd
    Server srv(argv[1], argv[2]);
    std::vector<struct pollfd> pfds;

    // Setup listen socket
    int listenfd = srv.setup_listen_socket(pfds);
    if(listenfd < 0) return -1;
    std::cout << "IRC server listening on port " << srv._port << " ...\n";

    // Main server loop
    while(true)
    {
        // Define the poll policy
        srv.set_poll_policy(pfds);

        // Monitor/Poll the fds in pfds
        int ready = poll(&pfds[0], pfds.size(), -1);
        if(ready < 0)
        {
            if(errno == EINTR) continue;
            std::cerr << "poll error: " << std::strerror(errno) << std::endl; 
        }

        // Accept new clients (if requests waiting)
        srv.accept_new(pfds, listenfd);

        // IO for clients
        for(size_t i = 1; i < pfds.size();)
        {
            int fd = pfds[i].fd;
            Client * client = srv.get_client(fd);
            if(!client)
            {
                i++;
                continue;
            }

            bool success = true;
            // Handle new message from client
            if (pfds[i].revents & POLLIN)
                success = srv.handle_client_input(*client, fd);
            if (!success)
            {
                Client::close_client(pfds, srv, i);
                continue;
            }
                
            // Handle sending messages to clients
            if((pfds[i].revents & POLLOUT) && !client->outbuf.empty())
            {
                success = srv.handle_client_output(*client, fd);
            }
            if (!success)
            {
                Client::close_client(pfds, srv, i);
                continue;
            }
            ++i;
        }
    }

    close(listenfd);
    return 0;
}