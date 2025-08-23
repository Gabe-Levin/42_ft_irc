#include "../Client.hpp"

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
ERR_NONICKNAMEGIVEN (431)     | NO          | No nickname given
ERR_ERRONEUSNICKNAME (432)    | NO          | Invalid nickname (illegal characters / too long / bad prefix)
ERR_NICKNAMEINUSE (433)       | NO          | Nickname already in use


Berni(28.08.): Maybe we need a solution for this one:
(Source Lea:) ERR_NICKCOLLISION (436) // not existant for US
Command Example:
  NICK Wiz                  ; Requesting the new nick "Wiz".*/

void Client::do_nick(std::istringstream &iss, Client &c)
{
    iss >> c.nick;
}