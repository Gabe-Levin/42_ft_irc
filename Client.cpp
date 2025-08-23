#include "Client.hpp"
#include "Server.hpp"
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

/*
	ERROR						| STATUS/DONE	| DISCRIPTIION
	ERR_UNKNOWNCOMMAND (421)	| NO			| Unknown command (parser/dispatcher can’t find handler
	ERR_NOTREGISTERED (451)		| NO			| Command before full registration (everything except PASS/NICK/USER/PING/QUIT)
	ERR_NEEDMOREPARAMS (461)	| NO			| Not enough parameters for the command (missing required argument))
*/

void Client::handle_cmd(Client &c, const std::string &line, Server &srv)
{
    std::istringstream iss(line);
    std::string cmd;

    iss >> cmd; //set the first word in the iss stream to cmd

    // Registration
    if(cmd == "PASS")
        c.do_pass(iss, srv, c);
    if(cmd == "NICK")
        c.do_nick(iss, c);
    if (cmd == "USER")  //TODO: User should have a specific format (ex. "USER myuser 0 * :John Smith")
        c.do_user(iss, c);
    if (c.password && !c.nick.empty() && !c.user.empty() && !c.registered) // Only do this when first registering
    {
        c.registered = true;
        c.outbuf += "Congrats! Client has successfully registered! \r\n";
    }

    // Cmds after registration
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

    // if(cmd == "KICK" || cmd == "INVITE" || cmd == "MODE")
    // {
    //     std::string channel_name;
    //     iss >> channel_name;

    //     Channel* channel = Channel::find_channel(channel_name, srv);
    //     if(channel == NULL)
    //     {                
    //         c.outbuf += "Could not find reference channel: " + channel_name + "\r\n";
    //         return;
    //     }
    //     Client * op = (*channel).find_operator(c.nick);
    //     if(op == NULL)
    //     {
    //         c.outbuf += "Sorry bud, you are not an operator. \r\n";
    //         return;
    //     }
        
    //     if(cmd == "KICK")
    //     {
    //         std::string nick;
    //         iss >> nick;
    //         if((*channel).kick_client(nick))
    //         {
    //             c.outbuf += "User " + nick + " could NOT be found in: " + channel_name + "\r\n";
    //             return;
    //         }
    //         else
    //             c.outbuf += "User " + nick + " has been kicked from " + channel_name + "\r\n";
    //         return;
    //     }
        
    //     if(cmd == "INVITE")
    //     {
    //         std::string channel;
    //         std::string nick;
    //         iss >> channel;
    //         iss >> nick;

    //         Channel *e_channel = (Channel::find_channel(channel, srv));
    //         if(e_channel != NULL)
    //         {
    //             c.outbuf += "Error\r\n";
    //             return;
    //         }

    //         Client *invitee = srv.get_client(nick);
    //         if (invitee != NULL)
    //         {
    //             c.outbuf += "Error\r\n";
    //             return;
    //         }

    //         (*e_channel).invite_list.push_back(invitee);
    //         c.outbuf += ":" + c.nick + "!" + c.nick + "@host"+ " INVITE " + invitee->nick + " :" + e_channel->_name + "\r\n";
    //         invitee->outbuf += ":" + c.nick + "!" + c.nick + "@host"+ " INVITE " + invitee->nick + " :" + e_channel->_name + "\r\n";
    //         return;
    //     }

    //     if(cmd == "MODE")
    //     {
    //         std::string flag;
    //         std::string msg;
    //         iss >> flag;

    //         if(flag.empty()) {
    //             c.outbuf += ":server 461 " + c.nick + " MODE :Not enough parameters\r\n";
    //             return;
    //         }
            
    //         // TODO: get actual host ip from socket after accept
    //         msg = ": " + c.nick + "!" + c.user + "@host" + " MODE " + channel->_name + " " + flag + "\r\n";
    //         if(flag == "+i") //Make channel invite-only
    //         {
    //             channel->invite_only = true;
    //             channel->broadcast(msg);
    //         }
    //         else if(flag == "-i") //Lift invite-only restriction
    //         {
    //             channel->invite_only = false;
    //             channel->broadcast(msg);
    //         }
    //         else if(flag == "+t")
    //         {
    //             channel->topic_restricted = true;
    //             channel->broadcast(msg);
    //         }
    //         else if(flag == "-t")
    //         {
    //             channel->topic_restricted = false;
    //             channel->broadcast(msg);
    //         }
    //         else if(flag == "+k") //Set channel password
    //         {
    //             std::string secretpwd;
    //             iss >> secretpwd;
    //             if(secretpwd.empty())
    //                 c.outbuf += ":server 461 " + c.nick + " MODE :Not enough parameters\r\n";
    //             else
    //             {
    //                 channel->secretpwd = secretpwd;
    //                 channel->broadcast(msg);
    //             }
    //         }
    //         else if(flag == "-k") //Lift channel password
    //         {
    //             channel->secretpwd.erase();
    //             channel->broadcast(msg);
    //         }
    //         else if(flag == "+o" || flag == "-o") //Make/Kick operators
    //         {
    //             std::string flags; 
    //             iss >> flags;  
    //             if (flags.size() != 2 || (flags[0] != '+' && flags[0] != '-') || flags[1] != 'o') {
    //                 c.outbuf += ":server 501 " + c.nick + " :Unknown MODE flag\r\n"; // ERR_UMODEUNKNOWNFLAG
    //                 return;
    //             }

    //             std::string nick;
    //             iss >> nick;
    //             if(nick.empty())
    //             {
    //                 c.outbuf += ":server 461 " + c.nick + " MODE :Not enough parameters\r\n";
    //                 return;
    //             }
    //             else if(flag[0]== '+')
    //             {
    //                 channel->make_operator(nick);
    //                 channel->broadcast(msg);
    //             }
    //             else if(flag[0]== '-')
    //             {
    //                 channel->kick_operator(nick);
    //                 channel->broadcast(msg);
    //             }
    //         }

    //         else if(flag == "+l") //Set max_clients
    //         {
    //             std::string max_clients_input; 
    //             iss >> max_clients_input;

    //             int max_clients = atoi(max_clients_input.c_str());
    //             channel->max_clients = max_clients;
    //             channel->broadcast(msg);
    //         }
    //         else if(flag == "-l") //Lift max_clients
    //         {
    //             channel->max_clients = 0;
    //             channel->broadcast(msg);
    //         }
    //     }

    //     return;
    // }