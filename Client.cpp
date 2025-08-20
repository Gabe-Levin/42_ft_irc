#include "Client.hpp"
#include "Server.hpp"

void Client::close_client(std::vector<struct pollfd>& pfds, std::map<int, Client>& clients, size_t idx)
{
    int fd = pfds[idx].fd;
    close(fd);
    clients.erase(fd);
    pfds.erase(pfds.begin() + idx);
}

bool Client::pop_line(std::string &buffer, std::string &line)
{
    std::string::size_type pos = buffer.find('\n');
    if (pos == std::string::npos) return false;
    line = buffer.substr(0, pos);
    // rm trailing \r
    if(!line.empty() && line[line.size()-1]== '\r')
        line.erase(line.size()-1);
    buffer.erase(0, pos + 1);
    return true;
}

void Client::accept_new(std::vector<struct pollfd> &pfds, std::map<int, Client> &clients, int listenfd)
{
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
            if(Server::set_nonblocking(cfd) < 0)
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
            clients[cfd].outbuf += "Welcome to the server. Please register your PASS, NICK, and USER\r\n";
        }
    }
}