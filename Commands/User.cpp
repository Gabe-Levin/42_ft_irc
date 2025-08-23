#include "../Client.hpp"

/*
    https://modern.ircdocs.horse/#user-message

ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | Not enough fields in USER command
ERR_ALREADYREGISTERED (462)   | NO          | USER after registration already completed

*/

void Client::do_user(std::istringstream &iss, Client &c)
{
    iss >> c.user;
}