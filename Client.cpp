#include "Client.hpp"
#include "Server.hpp"

//TODO: this also needs to close and erase the users from all channels and from the server
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

    if(cmd == "PASS")
    {
        std::string pass;
        iss >> pass;
        if(pass == srv._password)
        {
            c.password = true;
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
    //TODO: User should have a specific format (ex. "USER myuser 0 * :John Smith")
    else if (cmd == "USER")
    {
        iss >> c.user;
    }
    if (c.password && !c.nick.empty() && !c.user.empty() && !c.registered) // Only do this when first registering
    {
        c.registered = true;
        c.outbuf += "Congrats! Client has successfully registered! \r\n";
    }

    if(cmd == "JOIN")
    {
        std::string channel;
        if(!c.registered)
            c.outbuf += "You must register before you can join/create a channel \r\n";
        else
        {
            iss >> channel; //TODO: validate channel input
            Channel *existing_channel = Channel::find_channel(channel, srv);
            if (existing_channel == NULL)
            {
                
                Channel new_channel(channel);
                srv.channels.push_back(new_channel);
                srv.channels.back().clients.push_back(&c);

                c.outbuf += "Congrats, you have just created and joined a new channel '" + channel + "'!\r\n";
            }
            else
            {
                (*existing_channel).clients.push_back(&c);
                c.outbuf += "Congrats, you have just joined a new channel '" + channel + "'!\r\n";
            }
        }
    }

    if(cmd == "PRIVMSG")
    {
        std::string target;
        std::string message;
        iss >> target;

        std::getline(iss, message);
        if (!message.empty() && message[0] == ':')
            message.erase(0, 1);
        
        // Handle messages to a channel
        if(!target.empty() && target[0] == '#')
        { 
            Channel *existing_channel = Channel::find_channel(target, srv);
            if (existing_channel == NULL)
            {
                c.outbuf += "Target channel '" + target + "' does not exist. \r\n";
                return;
            }

            for(std::vector<Client*>::iterator it = existing_channel->clients.begin(); it != existing_channel->clients.end(); ++it)
            {
                if(c.user != (*it)->user) //:Alice!alice@127.0.0.1 PRIVMSG #42network :hello everyone!
                {
                    (*it)->outbuf += ":" + c.nick + " " + cmd + " " + target + message + "\r\n";
                }
            }
        }
        // Handle direct messages to another client
        else
        {
            for(std::vector<Client*>::iterator it = srv.clients.begin(); it != srv.clients.end(); ++it)
            {
                if(target == (*it)->nick)
                {
                    (*it)->outbuf += ":" + c.nick + " " + cmd + " " + target + message + "\r\n";
                }
            }
        }

    }

}