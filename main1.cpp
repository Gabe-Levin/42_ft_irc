// Minimal non-blocking echo server using poll(), C++98
// Build: make
// Run:   ./ircserv <port> <password>   (password unused for echo step)

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <cerrno>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) return -1;
    return 0;
}

static int create_listen_socket(const char* port) {
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags    = AI_PASSIVE;  // for bind

    struct addrinfo* res = 0;
    int gai = getaddrinfo(NULL, port, &hints, &res);
    if (gai != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(gai) << std::endl;
        return -1;
    }

    int listenfd = -1;
    for (struct addrinfo* rp = res; rp; rp = rp->ai_next) {
        int fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) continue;

        int yes = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            listenfd = fd;
            break;
        }
        close(fd);
    }
    freeaddrinfo(res);

    if (listenfd < 0) {
        std::cerr << "bind: failed on all addresses\n";
        return -1;
    }
    if (set_nonblocking(listenfd) < 0) {
        std::cerr << "fcntl nonblock failed\n";
        close(listenfd);
        return -1;
    }
    if (listen(listenfd, SOMAXCONN) < 0) {
        std::cerr << "listen failed: " << std::strerror(errno) << std::endl;
        close(listenfd);
        return -1;
    }
    return listenfd;
}

struct Client {
    int fd;
    std::string inbuf;   // bytes received, not yet processed into lines
    std::string outbuf;  // pending bytes to send (if send() would block)
    Client(): fd(-1) {}
    Client(int f): fd(f) {}
};

static void close_client(std::vector<struct pollfd>& pfds, std::map<int, Client>& clients, size_t idx) {
    int fd = pfds[idx].fd;
    close(fd);
    clients.erase(fd);
    pfds.erase(pfds.begin() + idx);
}

// Extract one complete IRC-style line (ending in \r\n or \n). Returns true if got a line.
static bool pop_line(std::string& buffer, std::string& line) {
    // Look for \n; strip optional preceding \r
    std::string::size_type pos = buffer.find('\n');
    if (pos == std::string::npos) return false;
    line = buffer.substr(0, pos);
    if (!line.empty() && line[line.size()-1] == '\r')
        line.erase(line.size()-1);
    buffer.erase(0, pos + 1);
    return true;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }
    const char* port = argv[1];

    int listenfd = create_listen_socket(port);
    if (listenfd < 0) return 1;

    std::vector<struct pollfd> pfds;
    pfds.push_back(pollfd());
    pfds[0].fd = listenfd;
    pfds[0].events = POLLIN;

    std::map<int, Client> clients;

    std::cout << "Echo server listening on port " << port << " ...\n";

    while (true) {
        // Update writable events depending on outbuf
        for (size_t i = 1; i < pfds.size(); ++i) {
            int fd = pfds[i].fd;
            std::map<int, Client>::iterator it = clients.find(fd);
            if (it != clients.end()) {
                pfds[i].events = POLLIN;
                if (!it->second.outbuf.empty())
                    pfds[i].events |= POLLOUT;
            }
        }

        int ready = poll(&pfds[0], pfds.size(), -1);
        if (ready < 0) {
            if (errno == EINTR) continue;
            std::cerr << "poll error: " << std::strerror(errno) << std::endl;
            break;
        }

        // 1) Accept new clients
        if (pfds[0].revents & POLLIN) {
            for (;;) {
                struct sockaddr_storage ss;
                socklen_t slen = sizeof(ss);
                int cfd = accept(listenfd, (struct sockaddr*)&ss, &slen);
                if (cfd < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    std::cerr << "accept error: " << std::strerror(errno) << std::endl;
                    break;
                }
                if (set_nonblocking(cfd) < 0) {
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

                // Greet (shows write path works)
                clients[cfd].outbuf += ":echo 001 * :Welcome to the tiny echo server\r\n";
            }
        }

        // 2) IO for clients
        for (size_t i = 1; i < pfds.size(); /* increment inside */) {
            int fd = pfds[i].fd;
            std::map<int, Client>::iterator it = clients.find(fd);
            if (it == clients.end()) { ++i; continue; }

            bool erased = false;

            // Readable?
            if (pfds[i].revents & POLLIN) {
                char buf[512];
                for (;;) {
                    int n = recv(fd, buf, sizeof(buf), 0);
                    if (n > 0) {
                        it->second.inbuf.append(buf, n);
                        // Process complete lines
                        std::string line;
                        while (pop_line(it->second.inbuf, line)) {
                            // ECHO logic: prepend simple prefix and echo back
                            std::string reply = ":echo NOTICE * :" + line + "\r\n";
                            it->second.outbuf += reply;
                        }
                    } else if (n == 0) {
                        // Peer closed
                        close_client(pfds, clients, i);
                        erased = true;
                        break;
                    } else {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        // Real error
                        close_client(pfds, clients, i);
                        erased = true;
                        break;
                    }
                }
            }
            if (erased) continue; // do not ++i, current index now refers to next element

            // Writable?
            if ((pfds[i].revents & POLLOUT) && !it->second.outbuf.empty()) {
                const char* data = it->second.outbuf.data();
                size_t left = it->second.outbuf.size();
                int n = send(fd, data, left, 0);
                if (n > 0) {
                    it->second.outbuf.erase(0, n);
                } else if (n < 0) {
                    if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
                        close_client(pfds, clients, i);
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
