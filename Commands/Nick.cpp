#include "../Client.hpp"
#include "../Messages.hpp"

bool isValidChar(char c)
{
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
      return true;

    if (c == '[' || c == ']' || c == '\\' || c == '^' || 
        c == '_' || c == '{' || c == '|' || c == '}')
      return true;

    if ((c >= '0' && c <= '9') || c == '-')
      return true;

    return false;
}

bool isValidFirstChar(char c) {
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        return true;
    }
    if (c == '[' || c == ']' || c == '\\' || c == '^' || 
        c == '_' || c == '{' || c == '|' || c == '}') {
        return true;
    }
    return false;
}

bool isValidNick(std::string nick)
{
  if(nick.length() > 9)
    return false;

  if(!isValidFirstChar(nick[0]))
    return false;

  for(size_t i = 1; i < nick.length(); i++)
  {
    if(!isValidChar(nick[i]))
      return false;
  }

  return true;
}

void Client::do_nick(std::istringstream &iss, Server &srv, Client &c)
{
    std::string nick;
    iss >> nick;
    if(!c.password)
    { 
        Msg::ERR_PASSWDMISMATCH(srv, c);
        c.toDisconnect = true;
        return;
    }

    if(nick.empty())
    {
      Msg::ERR_NONICKNAMEGIVEN(srv, c);
      return;
    }
    else if (!isValidNick(nick))
    {
      Msg::ERR_ERRONEUSNICKNAME(srv, c, nick);
      return;
    }
    else if (srv.is_nick_taken(nick))
    {
      Msg::ERR_NICKNAMEINUSE(srv, c, nick);
      return;
    }
    
    c.nick = nick;
    c.check_registration(srv);
    return;
}