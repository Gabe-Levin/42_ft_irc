#include "ErrorMsg.hpp"

std::string	errorMsg::createErrMsg(Client& cli, Server& serv, Channel& ch) {
	std::cerr << serv._port << this->_code << cli.nick
}