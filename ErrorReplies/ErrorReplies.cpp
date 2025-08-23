#include "ErrorReplies.hpp"

static void insert(std::map<int, ReplyPattern>& m, int code,
                   const char* name, const char* tmpl, int minp) {
    ReplyPattern rp;
    rp.code = code;
    rp.name = name;
    rp.tmpl = tmpl;
    rp.min_params = minp;
    m[code] = rp;
}



const std::map<int, ReplyPattern>& ErrorReplies::patterns() {
    static std::map<int, ReplyPattern> m;
    if (m.empty()) {
        // --- Auswahl gängiger Fehler (RFC-ähnlich) ---
        insert(m, 401, "ERR_NOSUCHNICK",      "{nick} :No such nick/channel", 1);
        insert(m, 403, "ERR_NOSUCHCHANNEL",   "{channel} :No such channel",    1);
        insert(m, 404, "ERR_CANNOTSENDTOCHAN","{channel} :Cannot send to channel",1);
        insert(m, 421, "ERR_UNKNOWNCOMMAND",  "{command} :Unknown command",    1);
        insert(m, 431, "ERR_NONICKNAMEGIVEN", ":No nickname given",            0);
        insert(m, 432, "ERR_ERRONEUSNICKNAME","{nick} :Erroneous nickname",    1);
        insert(m, 433, "ERR_NICKNAMEINUSE",   "{nick} :Nickname is already in use",1);
        insert(m, 441, "ERR_USERNOTINCHANNEL","{nick} {channel} :They aren't on that channel",2);
        insert(m, 442, "ERR_NOTONCHANNEL",    "{channel} :You're not on that channel",1);
        insert(m, 443, "ERR_USERONCHANNEL",   "{nick} {channel} :is already on channel",2);
        insert(m, 451, "ERR_NOTREGISTERED",   ":You have not registered",      0);
        insert(m, 461, "ERR_NEEDMOREPARAMS",  "{command} :Not enough parameters",1);
        insert(m, 462, "ERR_ALREADYREGISTERED",":You may not reregister",      0);
        insert(m, 464, "ERR_PASSWDMISMATCH",  ":Password incorrect",           0);
        insert(m, 471, "ERR_CHANNELISFULL",   "{channel} :Cannot join channel (+l)",1);
        insert(m, 472, "ERR_UNKNOWNMODE",     "{char} :is unknown mode char to me",1);
        insert(m, 473, "ERR_INVITEONLYCHAN",  "{channel} :Cannot join channel (+i)",1);
        insert(m, 475, "ERR_BADCHANNELKEY",   "{channel} :Cannot join channel (+k)",1);
        insert(m, 481, "ERR_NOPRIVILEGES",    ":Permission Denied- You're not an IRC operator",0);
        insert(m, 482, "ERR_CHANOPRIVSNEEDED","{channel} :You're not channel operator",1);
        insert(m, 501, "ERR_UMODEUNKNOWNFLAG",":Unknown MODE flag",            0);
        insert(m, 502, "ERR_USERSDONTMATCH",  ":Cannot change mode for other users",0);
    }
    return m;
}

std::string ErrorReplies::replace_all(std::string s,
                                      const std::string& key,
                                      const std::string& value) {
    if (key.empty()) return s;
    const std::string needle = std::string("{") + key + "}";
    std::string::size_type pos = 0;
    while ((pos = s.find(needle, pos)) != std::string::npos) {
        s.replace(pos, needle.size(), value);
        pos += value.size();
    }
    return s;
}

std::string ErrorReplies::int3(int code) {
    // Drei Ziffern als String (z. B. 4 6 4)
    std::ostringstream oss;
    if (code < 100) oss << '0';
    if (code < 10)  oss << '0';
    oss << code;
    return oss.str();
}

std::string ErrorReplies::format(int code,
                                 const std::string& server,
                                 const std::string& target,
                                 const std::map<std::string, std::string>& kv) {
    const std::map<int, ReplyPattern>& m = patterns();
    std::map<int, ReplyPattern>::const_iterator it = m.find(code);
    if (it == m.end()) {
        // Fallback, falls unbekannter Code
        std::ostringstream line;
        line << ":" << server << " " << int3(code) << " " << target
             << " :Unknown error\r\n";
        return line.str();
    }

    std::string body = it->second.tmpl;
    // Platzhalter ersetzen
    for (std::map<std::string, std::string>::const_iterator p = kv.begin();
         p != kv.end(); ++p) {
        body = replace_all(body, p->first, p->second);
    }

    std::ostringstream line;
    line << ":" << server << " " << int3(code) << " " << target << " " << body << "\r\n";
    return line.str();
}

void ErrorReplies::send(Client& to,
                        int code,
                        const std::string& server,
                        const std::string& target,
                        const std::map<std::string, std::string>& kv) {
    to.outbuf += format(code, server, target, kv);
}
