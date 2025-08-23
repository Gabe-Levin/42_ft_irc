#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>
#include <string>
#include <map>
#include "../Client.hpp"
#include "../Server.hpp"
#include "../Channel.hpp"

//ERRORS
/*
	//Replies
	331
	332
	333
	341
	353
	366

	//Errors
	401
	403
	404
	405
	409
	411
	412
	421
	431
	432
	433
	(436)
	441
	442
	443
	451
	461
	462
	464
	471
	472
	473
	474
	475
	476
	482

	501
	502
*/

enum errMsgType {
	ERR_NONICKNAMEGIVEN,
	NICK,
	USER,
	JOIN,
	PRIVMSG
};

class errorMsg {
	private:
		std::string _code;
		std::string _msg;

	public:
		std::string	createErrMsg(Client& cli, Server& serv, Channel& ch)
};

#endif