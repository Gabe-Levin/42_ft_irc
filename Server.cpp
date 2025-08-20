
#include "Server.hpp"

Server::Server(const char* port, const char *password)
{
    this->port = port;
    this->password = password;
}

int Server::set_nonblocking(int fd)
{
    int flags = fcntl(fd , F_GETFL, 0);
    if(flags == -1) return -1;
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) return -1;
    return 0;
}

int Server::create_listen_socket(const char* port)
{
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo* res = 0;
    int gai = getaddrinfo(NULL, port, &hints, &res);
    if(gai != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(gai) << std::endl;
        return -1;
    }

    int listenfd = -1;
    for (struct addrinfo* rp = res; rp; rp = rp->ai_next)
    {
        int fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) continue;

        // Rebind quickly after restarts
        // int yes = 1;
        // setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            listenfd = fd;
            break;
        }
        close(fd);
    }
    freeaddrinfo(res);

    if(listenfd < 0)
    {
        std::cerr << "bind: failed on all addresses\n";
        return -1;
    }
    if(set_nonblocking(listenfd) < 0)
    {
        std::cerr << "fnctl nonblock failed \n";
        close(listenfd);
        return -1;
    }
    if(listen(listenfd, SOMAXCONN) < 0)
    {
        std::cerr << "listen failed: " << std::strerror(errno) << std::endl;
        close(listenfd);
        return -1;
    }
    return listenfd;
}
