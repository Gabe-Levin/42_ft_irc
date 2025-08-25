#include "Client.hpp"
#include "Server.hpp"
#include "Messages.hpp"
#include <string>
#include <cstdlib> 

//TODO: this also needs to close and erase the users from all channels and from the server
void Client::close_client(std::vector<struct pollfd>& pfds, std::map<int, Client>& clients, size_t idx)
{
    int fd = pfds[idx].fd;
    std::cout << "Closing client fd: " << fd << std::endl; // Add this debug line
    close(fd);
    clients.erase(fd);
    pfds.erase(pfds.begin() + idx);
}

bool Client::pop_line(std::string &buffer, std::string &line)
{
    std::string::size_type pos = buffer.find('\n');
    if (pos == std::string::npos) return false;
    line = buffer.substr(0, pos);
    if(!line.empty() && line[line.size()-1]== '\r') // rm trailing \r
        line.erase(line.size()-1);
    buffer.erase(0, pos + 1);
    return true;
}

void Client::accept_new(std::vector<struct pollfd> &pfds, std::map<int, Client> &clients, Server &srv, int listenfd)
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

            clients.insert(std::make_pair(cfd, Client(cfd)));
            Client* cli = &clients.find(cfd)->second;

            struct pollfd p;
            std::memset(&p, 0, sizeof(p));
            p.fd = cfd;
            p.events = POLLIN;
            pfds.push_back(p);

            srv.clients.push_back(cli);

            // Greeting
            clients[cfd].outbuf += "Welcome to the server. Please register your PASS, NICK, and USER\r\n";
        }
    }
}

void Client::handle_cmd(Client &c, const std::string &line, Server &srv)
{
    std::istringstream iss(line);
    std::string cmd;

    iss >> cmd; //set the first word in the iss stream to cmd
    std::cout << "-------- " << cmd << " --------" << std::endl;

    // Registration
    if(cmd == "PASS")
        c.do_pass(iss, srv, c);
    if(cmd == "NICK")
        c.do_nick(iss, srv, c);
    if (cmd == "USER")
        c.do_user(iss, srv, c);
    if (c.password && !c.nick.empty() && !c.user.empty() && !c.registered) // Only do this when first registering
    {
        c.registered = true;
        Msg::RPL_WELCOME(srv, c);
        Msg::RPL_YOURHOST(srv, c);
        Msg::RPL_CREATED(srv, c);
        Msg::RPL_MYINFO(srv, c);
    }

    // User Cmds
    if(cmd == "JOIN")
        c.do_join(iss, srv, c);
    if(cmd == "PRIVMSG")
        c.do_privmsg(iss, srv, c);

    // Operator Cmds
    if(cmd == "TOPIC")
        c.do_topic(iss, srv, c);
    if(cmd == "KICK")
        c.do_kick(iss, srv, c);
    if(cmd == "INVITE")
        c.do_invite(iss, srv, c);
    if(cmd == "MODE")
        c.do_mode(iss, srv, c);
}