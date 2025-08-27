#include "Client.hpp"
#include "Server.hpp"
#include "Messages.hpp"
#include <string>
#include <cstdlib> 

// Canonical Form
Client::Client(): fd(-1), password(false), registered(false), toDisconnect(false){};

Client::Client(const Client& other): 
    fd(other.fd),
    password(other.password),
    registered(other.registered),
    toDisconnect(other.toDisconnect),
    inbuf(other.inbuf),
    outbuf(other.outbuf),
    nick(other.nick),
    user(other.user)
{}

Client& Client::operator=(const Client &other) 
{
    if (this != &other) {
        fd = other.fd;
        nick = other.nick;
        user = other.user;
        inbuf = other.inbuf;
        registered = other.registered;
        outbuf = other.outbuf;
    }
    return *this;
};

Client::~Client() {};

Client::Client(int f): fd(f) {};


void Client::close_client(std::vector<struct pollfd>& pfds, Server &srv, size_t idx)
{
    int fd = pfds[idx].fd;
    std::cout << "Closing client fd: " << fd << " at index: " << idx << std::endl;
    Client *c = srv.get_client(fd);
    for(std::vector<Channel>::iterator it = srv.channels.begin(); it != srv.channels.end(); it++)
        it->kick_client(c->nick);

    close(fd);
    srv.rm_client(fd);
    srv.clear_empty_channels();
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

bool Client::check_registration(Server &srv)
{
    if (password && !nick.empty() && !user.empty() && !registered) // Only do this when first registering
    {
        registered = true;
        Msg::RPL_WELCOME(srv, *this);
        Msg::RPL_YOURHOST(srv, *this);
        Msg::RPL_CREATED(srv, *this);
        Msg::RPL_MYINFO(srv, *this);
        return true;
    }
    return false;
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
    else if(cmd == "NICK")
        c.do_nick(iss, srv, c);
    else if (cmd == "USER")
        c.do_user(iss, srv, c);
    
    // Stop if not registered
    else if(!c.registered)
    {
        Msg::ERR_NOTREGISTERED(srv, c);
        return;
    }
    
    // User Cmds
    else if(cmd == "NAMES")
        c.do_names(iss, srv, c);
    else if(cmd == "PART")
        c.do_part(iss, srv, c);
    else if(cmd == "JOIN")
        c.do_join(iss, srv, c);
    else if(cmd == "PRIVMSG")
        c.do_privmsg(iss, srv, c);

    // Operator Cmds
    else if(cmd == "TOPIC")
        c.do_topic(iss, srv, c);
    else if(cmd == "KICK")
        c.do_kick(iss, srv, c);
    else if(cmd == "INVITE")
        c.do_invite(iss, srv, c);
    else if(cmd == "MODE")
        c.do_mode(iss, srv, c);

    else if(!cmd.empty())
        Msg::ERR_UNKNOWNCOMMAND(srv, c, cmd);
}