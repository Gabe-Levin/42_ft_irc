GENERAL
/*
	ERROR						| STATUS/DONE	| DISCRIPTIION
	ERR_UNKNOWNCOMMAND (421)	| NO			| Unknown command (parser/dispatcher can’t find handler
	ERR_NOTREGISTERED (451)		| NO			| Command before full registration (everything except PASS/NICK/USER/PING/QUIT)
	ERR_NEEDMOREPARAMS (461)	| NO			| Not enough parameters for the command (missing required argument))
*/


INVITE
	/*
(SOURCE LEA)
https://modern.ircdocs.horse/#invite-message
									done?
    RPL_INVITING (341)			=>	no
    ERR_NEEDMOREPARAMS (461)	=>	no
    ERR_NOSUCHCHANNEL (403)		=>	no
    ERR_NOTONCHANNEL (442)		=>	no
    ERR_CHANOPRIVSNEEDED (482)	=>	no
    ERR_USERONCHANNEL (443)		=>	no
	ERR_NOSUCHNICK (401)		=>	no

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | INVITE without <nick> or <channel>
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_CHANOPRIVSNEEDED (482)    | NO          | Only channel operators can invite
ERR_USERONCHANNEL (443)       | NO          | Target user is already in channel
ERR_NOSUCHNICK (401)          | NO          | Target nickname does not exist

*/

JOIN
/*
(SOURCE LEA)
    https://modern.ircdocs.horse/#join-message
									done?
    ERR_NOTREGISTERED (451)		=>	no
	ERR_NEEDMOREPARAMS (461)	=>	no
    ERR_NOSUCHCHANNEL (403) 	=>	no
    ERR_TOOMANYCHANNELS (405)	=> (LEA: ) we ignore that...
    ERR_BADCHANNELKEY (475)		=>	no
    x ERR_BANNEDFROMCHAN (474)	=> (LEA: )we don't ban
    ERR_CHANNELISFULL (471)		=> (LEA: )TODO: we ignore that... our limit is MAX_INT ? ==> nop
    ERR_INVITEONLYCHAN (473)	=>	no
    ERR_BADCHANMASK (476)		=>	(LEA: )???
    RPL_TOPIC (332)				=>	no
    RPL_TOPICWHOTIME (333)		=>	(LEA: )we don't do that (no history in our modest server)
    RPL_NAMREPLY (353)			=>	no
    RPL_ENDOFNAMES (366)		=>	no

(SOURCE CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | JOIN without channel
ERR_BADCHANMASK (476)         | NO          | Invalid channel name
ERR_INVITEONLYCHAN (473)      | NO          | JOIN to +i channel without invite
ERR_BADCHANNELKEY (475)       | NO          | JOIN with wrong key on +k channel
ERR_CHANNELISFULL (471)       | NO          | JOIN when +l channel limit is reached


*/

KICK
/*
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | KICK without <channel> or <nick>
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_NOTONCHANNEL (442)        | NO          | User is not on channel
ERR_USERNOTINCHANNEL (441)    | NO          | Target user is not in channel
ERR_CHANOPRIVSNEEDED (482)    | NO          | User is not channel operator

*/

MODE
/*
(SOURCE LEA)
If <target> is a nickname that does not exist on the network,
the ERR_NOSUCHNICK (401) numeric is returned. 
If <target> is a different nick than the user who sent the command, 
the ERR_USERSDONTMATCH (502) numeric is returned.
If <modestring> is not given, the RPL_UMODEIS (221) numeric is sent back
containing the current modes of the target user.
If <modestring> is given, the supplied modes will be applied, and a MODE
message will be sent to the user containing the changed modes. If one or more
modes sent are not implemented on the server, the server MUST apply the modes
that are implemented, and then send the ERR_UMODEUNKNOWNFLAG (501) in reply 
along with the MODE message.
option we are not implementing:
 o: Give/take channel operator privilege -can only be used on Channel
 i: Invisible User Mode

Error Replies:
							done?
ERR_NOSUCHNICK (401)		=>	no
ERR_UMODEUNKNOWNFLAG (501)	=>	no
ERR_USERSDONTMATCH (502)	=>	no

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_NEEDMOREPARAMS (461)      | NO          | Missing arguments for mode (+k, +l, +o/-o)
ERR_UNKNOWNMODE (472)         | NO          | Unknown channel mode flag
ERR_CHANOPRIVSNEEDED (482)    | NO          | User is not channel operator
ERR_USERNOTINCHANNEL (441)    | NO          | Target user for +o/-o not in channel

*/

NICK
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

PART 
/*- Team Lea didn't go for this one
(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | PART without channel
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_NOTONCHANNEL (442)        | NO          | User is not on the channel
*/

PASS
/*
    https://modern.ircdocs.horse/#pass-message
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | PASS without password
ERR_ALREADYREGISTERED (462)   | NO          | PASS after registration already completed
ERR_PASSWDMISMATCH (464)      | NO          | Wrong password (disconnect after sending error)

*/

PING/PONG
/*
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NOORIGIN (409)            | NO          | PING without parameter
ERR_NEEDMOREPARAMS (461)      | NO          | PONG without parameter (optional)
*/

PRIVMSG
/*
    https://modern.ircdocs.horse/#privmsg-message

ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NORECIPIENT (411)         | NO          | PRIVMSG without recipient
ERR_NOTEXTTOSEND (412)        | NO          | PRIVMSG without message
ERR_NOSUCHNICK (401)          | NO          | Target nickname does not exist
ERR_NOSUCHCHANNEL (403)       | NO          | Target channel does not exist - !! Lea wasn"t going for this one
ERR_CANNOTSENDTOCHAN (404)    | NO          | Cannot send to channel (e.g. moderated +m)


	(??? Don't know if necessary (berni, 28.08.)
	(Sourc: Lea)RPL_AWAY = 301 // not doing that one anymore, doesn't make sense, as we don't register users

*/

QUIT
/*
Berni (23.08.): Nice to have. Do we need this????
(SOURCE LEA)
https://modern.ircdocs.horse/#quit-message
example: QUIT :Gone to have lunch 
may relpies with ERROR message:
https://modern.ircdocs.horse/#error-message
does a broadcast of the quit:
example: :dan-!d@localhost QUIT :Quit: Bye for now!

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
(none)                        | -           | QUIT normally has no errors, just closes connection

*/

TOPIC
/*

(SOURCE LEA)
https://modern.ircdocs.horse/#topic-message
										done?
    ERR_NEEDMOREPARAMS (461)		=>	no
    ERR_NOSUCHCHANNEL (403)			=>	no
    ERR_NOTONCHANNEL (442)			=>	no
    ERR_CHANOPRIVSNEEDED (482)		=>	no
    RPL_NOTOPIC (331)				=>	no
    RPL_TOPIC (332)					=>	no
	(??? Don't know if necessary (berni, 28.08.)
    (Sourc: Lea)RPL_TOPICWHOTIME (333)		=> NO : we dont hav history in our server !! (Source: Lea)

(CHATTY)
ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | TOPIC without channel
ERR_NOSUCHCHANNEL (403)       | NO          | Channel does not exist
ERR_CHANOPRIVSNEEDED (482)    | NO          | Channel has +t and user is not channel operator
ERR_NOTONCHANNEL (442)        | NO          | User not on channel (if you enforce membership for viewing/setting topic)


*/

USER
/*
    https://modern.ircdocs.horse/#user-message

ERROR                         | STATUS/DONE | DESCRIPTION
ERR_NEEDMOREPARAMS (461)      | NO          | Not enough fields in USER command
ERR_ALREADYREGISTERED (462)   | NO          | USER after registration already completed

*/

WHO
/*
    https://modern.ircdocs.horse/#join-message
									done?
    ERR_NOTREGISTERED (451)		=>	no
	ERR_NEEDMOREPARAMS (461)	=>	no
    ERR_NOSUCHCHANNEL (403) 	=>	no
    RPL_NAMREPLY (353)			=>	no
    RPL_ENDOFNAMES (366)		=>	no
*/