#include "../Client.hpp"
#include "../Messages.hpp"

void Client::do_list(std::istringstream &iss, Server &srv, Client &c)
{
    (void)iss;

    Msg::RPL_LISTSTART(srv, c);

    // Iterate over all channels on the server
    for (std::vector<Channel>::iterator it = srv.channels.begin();
         it != srv.channels.end(); ++it)
    {
        Channel &ch = *it;

        // Visible user count
        std::ostringstream num;
        num << ch.clients.size();

        // Topic may be empty string
        std::string topic = ch.topic;

        Msg::RPL_LIST(srv, c, ch._name, ch.clients.size(), topic);
    }

    Msg::RPL_LISTEND(srv, c);
}
