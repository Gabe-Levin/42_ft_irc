#ifndef MSG_HPP
#define MSG_HPP

#include <string>
#include <vector>
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

namespace Msg {

// Generic broadcasts
inline void BROADCAST_JOIN(Server& srv, Client& c, Channel& channel)
{
    std::string msg;
    msg = ":" + c.nick + "!" + c.user + "@" + srv._name + " JOIN " + channel._name + "\r\n";
    channel.broadcast(msg);
}

inline void CHANNEL(Server& srv, Client& c, Channel& channel, std::string msg)
{
    std::cout << "inside the channel broadcast" << std::endl;
    for(std::vector<Client*>::iterator it = channel.clients.begin(); it != channel.clients.end(); ++it)
        {
            if(c.nick != (*it)->nick) // broadcast to all but the sender
            {
                (*it)->outbuf += ":" + c.nick + "!" + c.user + "@" + srv._name
                        + " PRIVMSG " + channel._name + " :" + msg + "\r\n";
            }
        }
}

inline void CLIENT(Server& srv, Client& c, std::string target, std::string msg)
{
     for(std::vector<Client*>::iterator it = srv.clients.begin(); it != srv.clients.end(); ++it)
        {
            if(target == (*it)->nick)
            {
                (*it)->outbuf += ":" + c.nick + " " + "PRIVMSG" + " " + target + msg + "\r\n";
            }
        }
}


// ------------------ REPLIES (001, 002, 003, 004, 324, 331, 332, 341, 353, 366) ------------------

inline void RPL_WELCOME(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 001 " + c.nick +
                 " :Welcome to the IRC network " + c.nick + "!" + c.user + "@" + srv._name + "\r\n";
}

inline void RPL_YOURHOST(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 002 " + c.nick +
                 " :Your host is " + srv._name + "\r\n";
}

inline void RPL_CREATED(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 003 " + c.nick +
                 " :This server was created " + srv._created + "\r\n";
}

// Typical MYINFO: <servername> <version> <usermodes> <chanmodes> [chanmodes-with-params]
inline void RPL_MYINFO(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 004 " + c.nick + " " +
                 srv._name + " 1.0 - itkol kl\r\n";
}

// 324: <channel> <modestring> [mode params...]
inline void RPL_CHANNELMODEIS(Server& srv, Client& c, Channel& ch,
                               const std::string& modeString,
                               const std::string& modeParams = "") {
    c.outbuf += ":" + srv._name + " 324 " + c.nick + " " +
                 ch._name + " " + modeString +
                 (modeParams.empty() ? "" : " " + modeParams) + "\r\n";
}
inline void RPL_CHANNELMODEIS(Server& srv, Client& c, const std::string& chan,
                               const std::string& modeString,
                               const std::string& modeParams = "") {
    c.outbuf += ":" + srv._name + " 324 " + c.nick + " " +
                 chan + " " + modeString +
                 (modeParams.empty() ? "" : " " + modeParams) + "\r\n";
}

// 331: <channel> :No topic is set
inline void RPL_NOTOPIC(Server& srv, Client& c, Channel& ch) {
    c.outbuf += ":" + srv._name + " 331 " + c.nick + " " +
                 ch._name + " :No topic is set\r\n";
}

// 332: <channel> :<topic>
inline void RPL_TOPIC(Server& srv, Client& c, Channel& channel) {
    c.outbuf += ":" + srv._name + " 332 " + c.nick + " " +
                 channel._name + " :" + channel.topic + "\r\n";
}

// 341: <nick> <channel>  (RPL_INVITING)
inline void RPL_INVITING(Server& srv, Client& c, const std::string& invitedNick, Channel& ch) {
    c.outbuf += ":" + srv._name + " 341 " + c.nick + " " +
                 invitedNick + " " + ch._name + "\r\n";
}
inline void RPL_INVITING(Server& srv, Client& c, const std::string& invitedNick, const std::string& chan) {
    c.outbuf += ":" + srv._name + " 341 " + c.nick + " " +
                 invitedNick + " " + chan + "\r\n";
}

// 353: <symbol> <channel> :name1 name2 ...
inline void RPL_NAMREPLY(Server& srv, Client& c, Channel& channel) {
    std::string list;
    for(std::vector<Client*>::iterator it = channel.clients.begin(); it != channel.clients.end(); ++it)
    {
        if(channel.is_operator((*it)->nick))
            list += "@";
        list += (*it)->nick + " ";
    }
    c.outbuf += ":" + srv._name + " 353 " + c.nick + " = " + channel._name + " :" + list + "\r\n";
}

// 366: <channel> :End of /NAMES list.
inline void RPL_ENDOFNAMES(Server& srv, Client& c, Channel& channel) {
    c.outbuf += ":" + srv._name + " 366 " + c.nick + " " +
                 channel._name + " :End of /NAMES list.\r\n";
}


// ------------------ ERRORS (401,403,404,409,411,412,421,431,432,433,436,442,443,451,461,462,464,471..476,481,482,501,502) ------------------

inline void ERR_NOSUCHNICK(Server& srv, Client& c, const std::string &target) {
    c.outbuf += ":" + srv._name + " 401 " + c.nick + " " + target +
                 " :No such nick/channel\r\n";
}

inline void ERR_NOSUCHCHANNEL(Server& srv, Client& c, std::string channel_name) {
    c.outbuf += ":" + srv._name + " 403 " + c.nick + " " + channel_name +
                 " :No such channel\r\n";
}

inline void ERR_CANNOTSENDTOCHAN(Server& srv, Client& c, Channel& ch) {
    c.outbuf += ":" + srv._name + " 404 " + c.nick + " " + ch._name +
                 " :Cannot send to channel\r\n";
}

inline void ERR_NOORIGIN(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 409 " + c.nick +
                 " :No origin specified\r\n";
}

inline void ERR_NORECIPIENT(Server& srv, Client& c, const std::string& commandName) {
    c.outbuf += ":" + srv._name + " 411 " + c.nick +
                 " :No recipient given (" + commandName + ")\r\n";
}

inline void ERR_NOTEXTTOSEND(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 412 " + c.nick +
                 " :No text to send\r\n";
}

inline void ERR_UNKNOWNCOMMAND(Server& srv, Client& c, const std::string& cmd) {
    c.outbuf += ":" + srv._name + " 421 " + c.nick + " " + cmd +
                 " :Unknown command\r\n";
}

inline void ERR_NONICKNAMEGIVEN(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 431 " + c.nick +
                 " :No nickname given\r\n";
}

inline void ERR_ERRONEUSNICKNAME(Server& srv, Client& c, const std::string& badNick) {
    c.outbuf += ":" + srv._name + " 432 " + c.nick + " " + badNick +
                 " :Erroneous nickname\r\n";
}

inline void ERR_NICKNAMEINUSE(Server& srv, Client& c, const std::string& inUseNick) {
    c.outbuf += ":" + srv._name + " 433 " + c.nick + " " + inUseNick +
                 " :Nickname is already in use\r\n";
}

inline void ERR_NICKCOLLISION(Server& srv, Client& c, const std::string& collidingNick) {
    c.outbuf += ":" + srv._name + " 436 " + c.nick + " " + collidingNick +
                 " :Nickname collision\r\n";
}

inline void ERR_NOTONCHANNEL(Server& srv, Client& c, Channel& ch) {
    c.outbuf += ":" + srv._name + " 442 " + c.nick + " " + ch._name +
                 " :You're not on that channel\r\n";
}
inline void ERR_NOTONCHANNEL(Server& srv, Client& c, const std::string& chan) {
    c.outbuf += ":" + srv._name + " 442 " + c.nick + " " + chan +
                 " :You're not on that channel\r\n";
}

inline void ERR_USERONCHANNEL(Server& srv, Client& c, const std::string& nickOn, Channel& ch) {
    c.outbuf += ":" + srv._name + " 443 " + c.nick + " " + nickOn + " " + ch._name +
                 " :is already on channel\r\n";
}
inline void ERR_USERONCHANNEL(Server& srv, Client& c, const std::string& nickOn, const std::string& chan) {
    c.outbuf += ":" + srv._name + " 443 " + c.nick + " " + nickOn + " " + chan +
                 " :is already on channel\r\n";
}

inline void ERR_NOTREGISTERED(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 451 " + c.nick +
                 " :You have not registered\r\n";
}

inline void ERR_NEEDMOREPARAMS(Server& srv, Client& c, const std::string& cmd) {
    c.outbuf += ":" + srv._name + " 461 " + c.nick + " " + cmd +
                 " :Not enough parameters\r\n";
}

inline void ERR_ALREADYREGISTERED(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 462 " + c.nick +
                 " :You may not reregister\r\n";
}

inline void ERR_PASSWDMISMATCH(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 464 " + c.nick +
                 " :Password mismatch\r\n";
}

inline void ERR_CHANNELISFULL(Server& srv, Client& c, Channel& ch) {
    c.outbuf += ":" + srv._name + " 471 " + c.nick + " " + ch._name +
                 " :Cannot join channel (+l)\r\n";
}
inline void ERR_CHANNELISFULL(Server& srv, Client& c, const std::string& chan) {
    c.outbuf += ":" + srv._name + " 471 " + c.nick + " " + chan +
                 " :Cannot join channel (+l)\r\n";
}

inline void ERR_UNKNOWNMODE(Server& srv, Client& c, const std::string& modeChar) {
    c.outbuf += ":" + srv._name + " 472 " + c.nick + " " + modeChar +
                 " :is unknown mode char to me\r\n";
}

inline void ERR_INVITEONLYCHAN(Server& srv, Client& c, Channel& ch) {
    c.outbuf += ":" + srv._name + " 473 " + c.nick + " " + ch._name +
                 " :Cannot join channel (+i)\r\n";
}
inline void ERR_INVITEONLYCHAN(Server& srv, Client& c, const std::string& chan) {
    c.outbuf += ":" + srv._name + " 473 " + c.nick + " " + chan +
                 " :Cannot join channel (+i)\r\n";
}

inline void ERR_BANNEDFROMCHAN(Server& srv, Client& c, Channel& ch) {
    c.outbuf += ":" + srv._name + " 474 " + c.nick + " " + ch._name +
                 " :Cannot join channel (+b)\r\n";
}
inline void ERR_BANNEDFROMCHAN(Server& srv, Client& c, const std::string& chan) {
    c.outbuf += ":" + srv._name + " 474 " + c.nick + " " + chan +
                 " :Cannot join channel (+b)\r\n";
}

inline void ERR_BADCHANNELKEY(Server& srv, Client& c, Channel& ch) {
    c.outbuf += ":" + srv._name + " 475 " + c.nick + " " + ch._name +
                 " :Cannot join channel (+k)\r\n";
}
inline void ERR_BADCHANNELKEY(Server& srv, Client& c, const std::string& chan) {
    c.outbuf += ":" + srv._name + " 475 " + c.nick + " " + chan +
                 " :Cannot join channel (+k)\r\n";
}

inline void ERR_BADCHANMASK(Server& srv, Client& c, const std::string& badMask) {
    c.outbuf += ":" + srv._name + " 476 " + c.nick + " " + badMask +
                 " :Bad Channel Mask\r\n";
}

inline void ERR_NOPRIVILEGES(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 481 " + c.nick +
                 " :Permission Denied- You're not an IRC operator\r\n";
}

inline void ERR_CHANOPRIVSNEEDED(Server& srv, Client& c, Channel& ch) {
    c.outbuf += ":" + srv._name + " 482 " + c.nick + " " + ch._name +
                 " :You're not channel operator\r\n";
}
inline void ERR_CHANOPRIVSNEEDED(Server& srv, Client& c, const std::string& chan) {
    c.outbuf += ":" + srv._name + " 482 " + c.nick + " " + chan +
                 " :You're not channel operator\r\n";
}

inline void ERR_UMODEUNKNOWNFLAG(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 501 " + c.nick +
                 " :Unknown MODE flag\r\n";
}

inline void ERR_USERSDONTMATCH(Server& srv, Client& c) {
    c.outbuf += ":" + srv._name + " 502 " + c.nick +
                 " :Cannot change mode for other users\r\n";
}

} // namespace msg

#endif // MSG_HPP
