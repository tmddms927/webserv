#include "Server.hpp"

Server::Server(std::vector<servers> s, Config const & c) : config(s), global_config(c.getGlobal()) {
	this->kq_timeout.tv_nsec = 0;
	this->kq_timeout.tv_sec = 0;
}
