#ifndef ERROR_REPLIES_HPP
#define ERROR_REPLIES_HPP

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include "../Client.hpp"

struct ReplyPattern {
    int         code;
    std::string name;   // z.B. "ERR_NOSUCHNICK"
    std::string tmpl;   // z.B. "{nick} :No such nick/channel"
    int         min_params; // grobe Sicherheit, z.B. 1 für {nick}
};

class ErrorReplies {
public:
    // Liefert Singleton-Map der Patterns
    static const std::map<int, ReplyPattern>& patterns();

    // Baut die komplette IRC-Zeile (inkl. CRLF).
    // server  = "irc.example.com"
    // target  = der Nick des Empfängers (der, der den Fehler bekommt)
    // kv      = Platzhalter -> Wert (z.B. "nick" -> "Bob", "channel" -> "#42")
    static std::string format(int code,
                              const std::string& server,
                              const std::string& target,
                              const std::map<std::string, std::string>& kv);

    // Convenience: direkt in den Client-Puffer schreiben
    static void send(Client& to,
                     int code,
                     const std::string& server,
                     const std::string& target,
                     const std::map<std::string, std::string>& kv);

private:
    static std::string int3(int code);
    static std::string replace_all(std::string s,
                                   const std::string& key,
                                   const std::string& value);
};

#endif // ERROR_REPLIES_HPP
