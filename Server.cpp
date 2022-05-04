#include "Server.hpp"

Server::Server(Config const & config) : servers(s) {
	this->kq_timeout.tv_nsec = 0;
	this->kq_timeout.tv_sec = 0;
}
