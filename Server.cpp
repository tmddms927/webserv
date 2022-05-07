#include "Server.hpp"

/*
** default constructor
*/
Server::Server(Config const & c) : config(c.getConfig()), global_config(c.getGlobal()) {
	this->kq_timeout.tv_nsec = 0;
	this->kq_timeout.tv_sec = 0;
}
