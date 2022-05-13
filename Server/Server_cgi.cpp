#include "Server.hpp"

/*
** write to cgi
*/
void Server::writeCGI() {
    uintptr_t client_fd;

    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!write" << std::endl;
    client_fd = cgi_fd[curr_event->ident];
    clients[client_fd].cgi_write(REQUEST_BODY_MAX_SIZE);
}

void Server::readCGI() {
    uintptr_t client_fd;

    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!read" << std::endl;
    client_fd = cgi_fd[curr_event->ident];
    clients[client_fd].cgi_read(REQUEST_BODY_MAX_SIZE);
	setResDefaultHeaderField(client_fd);
    change_events(client_fd, EVFILT_READ, EV_ENABLE);
	checkKeepAlive();
	clients[curr_event->ident].resetHTTP();
}
