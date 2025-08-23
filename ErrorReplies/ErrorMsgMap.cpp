#include "ErrorMsg.hpp"

const	std::map<errMsgType, errorMsg>& getErrorMap() {
	
	static std::map<errMsgType, errorMsg> m;
	if (m.empty()){
	//GENERAL
		m[] = createErrMsg();
	//INVITE
		m[ERR_NONICKNAMEGIVEN] = createErrMsg("431","No nickname given");
	//NICK
		m[ERR_NONICKNAMEGIVEN] = createErrMsg("431","No nickname given");
		m[ERR_ERRONEUSNICKNAME] = createErrMsg("432", "Invalid nickname (illegal characters / too long / bad prefix)");
		m[ERR_NICKNAMEINUSE] = creaeErrMsg("433", "Nickname already in use")
	}
}

//CHATTY
const std::map<errMsgType, errorMsg>& getErrorMap() {
    static std::map<errMsgType, errorMsg> m;
    if (m.empty()) {
        // GENERAL
        m[ERR_UNKNOWNCOMMAND]   = createErrMsg("421", "Unknown command");
        m[ERR_NOTREGISTERED]    = createErrMsg("451", "You have not registered");
        m[ERR_NEEDMOREPARAMS]   = createErrMsg("461", "Not enough parameters");

        // PASS
        m[ERR_ALREADYREGISTERED] = createErrMsg("462", "You may not reregister");
        m[ERR_PASSWDMISMATCH]    = createErrMsg("464", "Password incorrect");

        // NICK
        m[ERR_NONICKNAMEGIVEN]  = createErrMsg("431", "No nickname given");
        m[ERR_ERRONEUSNICKNAME] = createErrMsg("432", "Erroneous nickname");
        m[ERR_NICKNAMEINUSE]    = createErrMsg("433", "Nickname is already in use");

        // USER
        // (already covered by ERR_ALREADYREGISTERED, ERR_NEEDMOREPARAMS)

        // PRIVMSG
        m[ERR_NORECIPIENT]      = createErrMsg("411", "No recipient given");
        m[ERR_NOTEXTTOSEND]     = createErrMsg("412", "No text to send");
        m[ERR_NOSUCHNICK]       = createErrMsg("401", "No such nick/channel");
        m[ERR_NOSUCHCHANNEL]    = createErrMsg("403", "No such channel");
        m[ERR_CANNOTSENDTOCHAN] = createErrMsg("404", "Cannot send to channel");

        // JOIN
        m[ERR_BADCHANMASK]      = createErrMsg("476", "Bad channel mask");
        m[ERR_INVITEONLYCHAN]   = createErrMsg("473", "Cannot join channel (+i)");
        m[ERR_BADCHANNELKEY]    = createErrMsg("475", "Cannot join channel (+k)");
        m[ERR_CHANNELISFULL]    = createErrMsg("471", "Cannot join channel (+l)");

        // PART
        m[ERR_NOTONCHANNEL]     = createErrMsg("442", "You're not on that channel");

        // TOPIC
        m[ERR_CHANOPRIVSNEEDED] = createErrMsg("482", "You're not channel operator");

        // MODE
        m[ERR_UNKNOWNMODE]      = createErrMsg("472", "Unknown MODE flag");
        m[ERR_USERNOTINCHANNEL] = createErrMsg("441", "They aren't on that channel");

        // INVITE
        m[ERR_USERONCHANNEL]    = createErrMsg("443", "User is already on channel");

        // KICK
        // (uses ERR_USERNOTINCHANNEL, ERR_NOTONCHANNEL, ERR_CHANOPRIVSNEEDED above)

        // PING / PONG
        m[ERR_NOORIGIN]         = createErrMsg("409", "No origin specified");
    }
    return m;
}
