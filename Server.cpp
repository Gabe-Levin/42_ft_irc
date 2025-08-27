
#include "Server.hpp"
#include "Messages.hpp"


// Canonical Form
Server::Server(const char* port, const char *password)
{
    this->_port = port;
    this->_password = password;
    this->_name = get_server_name();
    this->_created = get_date_str();
}

Server::Server(const Server& other) 
    : _name(other._name), 
      _port(other._port), 
      _password(other._password),
      _created(other._created),
      channels(other.channels),
      clients(other.clients)
{}

Server& Server::operator=(const Server& other) 
{
    if (this == &other) {
        return *this;
    }
    
    _name = other._name;
    _port = other._port;
    _password = other._password;
    _created = other._created;
    channels = other.channels;
    clients = other.clients;
    
    return *this;
}

Server::~Server() {}


// Simple Methods
bool Server::is_valid_input(int argc, char** argv)
{
    if(argc!= 3)
    {
        std::cerr << "Usage : ./ircserv <port> <password>" << std::endl;
        return false;
    }

    int port;
    std::istringstream iss(argv[1]);
    if (!(iss >> port) || port <= 0 || port > 65535) {
        std::cerr << "Error: invalid port number" << std::endl;
        return false;
    }

    std::string password = argv[2];
    if (password.empty() || password.size() > 10) {
        std::cerr << "Error: password must not be empty or greater than 10 characters" << std::endl;
        return false;   
    }
    return true;
}

std::string Server::get_date_str() {
    time_t now = time(NULL);
    char buf[128];
    strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y", localtime(&now));
    return std::string(buf);
}

int Server::set_nonblocking(int fd)
{
    int flags = fcntl(fd , F_GETFL, 0);
    if(flags == -1) return -1;
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) return -1;
    return 0;
}


Client* Server::get_client(std::string nick)
{
    for(std::list<Client>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if(it->nick == nick)
        return &(*it);
    }
    return NULL;
}

Client* Server::get_client(int fd)
{
    for(std::list<Client>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if(it->fd == fd)
        return &(*it);
    }
    return NULL;
}

Channel* Server::find_channel(std::string name)
{
    for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        if(it->_name == name)
        return &(*it);
    }
    return NULL;
}

bool Server::is_client(std::string nick)
{
    for(std::list<Client>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if(it->nick == nick)
        return true;
    }
    return false;
}

bool Server::rm_client(int fd)
{
    for(std::list<Client>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if(it->fd == fd)
        {
            clients.erase(it);
            return true;
        }
    }
    return false;
}

bool Server::is_nick_taken(std::string nick)
{
    for(std::list<Client>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if(it->nick == nick)
        return true;
    }
    return false;
}

std::string Server::get_server_name()
{
    // Uncomment to show actual servername

    // char buf[256];
    // if (gethostname(buf, sizeof(buf)) != 0)
    //     return "unknown";
    // return std::string(buf);
    return "host";
}

void Server::clear_empty_channels()
{
  for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        if(it->clients.size() == 0)
        {
            channels.erase(it);
            return;
        }
    }
}


/////////////////////
// Complex methods //
/////////////////////
void  Server::set_poll_policy(std::vector<struct pollfd> &pfds)
{
    for(size_t i = 0; i < pfds.size(); ++i)
    {
        int fd = pfds[i].fd;
        Client * c1 = get_client(fd);

        if(c1 != NULL)
        {
            pfds[i].events = POLLIN; // always read
            if(!c1->outbuf.empty())
                pfds[i].events |= POLLOUT; // write only if msg pending
        }
    }
}

int Server::setup_listen_socket(std::vector<struct pollfd> &pfds)
{
    int listenfd = create_listen_socket();
        if(listenfd < 0) return -1;
    pfds.push_back(pollfd());
    pfds[0].fd = listenfd;
    pfds[0].events = POLLIN;

    return listenfd;
}
void Server::accept_new(std::vector<struct pollfd> &pfds, int listenfd)
{
    if(pfds[0].revents & POLLIN)
    {
        if(listenfd < 0)
        {
            std::cerr << "Invalid listen fd: " << listenfd << std::endl;
            return;
        }
        
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
            
            struct pollfd p;
            std::memset(&p, 0, sizeof(p));
            p.fd = cfd;
            p.events = POLLIN;
            pfds.push_back(p);
            
            clients.push_back(Client(cfd));
            Client *c = get_client(cfd);
            
            (*c).outbuf += "Welcome to the server. Please register your PASS, NICK, and USER\r\n";
        }
    }
    
}

int Server::create_listen_socket()
{
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo* res = 0;
    int gai = getaddrinfo(NULL, _port, &hints, &res);
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
        int yes = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

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

bool Server::handle_client_input(Server &srv, Client &client, int &fd)
{
    char buf[512];
    while(true)
    {
        int n = recv(fd, buf, sizeof(buf), 0);
        if(n > 0)
        {
            client.inbuf.append(buf, n);
            std::string line;
            while(Client::pop_line(client.inbuf, line))
            {
                Client::handle_cmd(client, line, *this);
            }
        }
        else if (n == 0)  // Peer closed. ie. nothing to read
        {
            Msg::BROADCAST_QUIT(srv, client, "");
            client.toDisconnect = true;
            return false;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
                return true; // stop reading for now, keep the client
            return false; // Fatal socket error
        }
    }
}

bool Server::handle_client_output(Client &client, int &fd)
{
    const char* data = client.outbuf.data();
    size_t leftover = client.outbuf.size();
    int n = send(fd, data, leftover, 0);
    if (n > 0)
    {
        client.outbuf.erase(0, n);

        // Close client check
        if (client.outbuf.empty() && client.toDisconnect)
            return false;
    }
    else if (n < 0)
    {
        if(!(errno == EAGAIN || errno == EWOULDBLOCK)) 
            return false;
    }
    return true;
}