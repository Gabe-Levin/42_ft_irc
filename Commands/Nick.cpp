#include "../Client.hpp"
#include "../Messages.hpp"


/*
The NICK command is used to give the client a nickname or change the previous one.
If the server receives a NICK command from a client where the desired nickname is 
already in use on the network, it should issue an ERR_NICKNAMEINUSE numeric and 
ignore the NICK command.
If the server does not accept the new nickname supplied by the client as valid 
(for instance, due to containing invalid characters), 
it should issue an ERR_ERRONEUSNICKNAME numeric and ignore the NICK command. 
Servers MUST allow at least all alphanumerical characters, square and curly 
brackets ([]{}), backslashes (\), and pipe (|) characters in nicknames, and 
MAY disallow digits as the first character. 
Servers MAY allow extra characters, as long as they do not introduce ambiguity 
in other commands, including:
no leading # character or other character advertized in CHANTYPES
no leading colon (:)
no ASCII space
If the server does not receive the <nickname> parameter with the NICK command,
it should issue an ERR_NONICKNAMEGIVEN numeric and ignore the NICK command.
The NICK message may be sent from the server to clients to acknowledge their 
NICK command was successful, and to inform other clients about the change of 
nickname. In these cases, the <source> of the message will be the old nickname 
[ [ "!" user ] "@" host ] of the user who is changing their nickname.

ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NONICKNAMEGIVEN (431)     | YES          | No nickname given
ERR_ERRONEUSNICKNAME (432)    | YES          | Invalid nickname (illegal characters / too long / bad prefix)
ERR_NICKNAMEINUSE (433)       | YES          | Nickname already in use


Berni(28.08.): Maybe we need a solution for this one:
(Source Lea:) ERR_NICKCOLLISION (436) // not existant for US
Command Example:
  NICK Wiz                  ; Requesting the new nick "Wiz".*/


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
    return;
}