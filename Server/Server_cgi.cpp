#include "Server.hpp"

/*
** write to cgi
*/
void Server::writeCGI() {
    uintptr_t client_fd;

    client_fd = cgi_fd[curr_event->ident];
    if (clients[client_fd].cgi_write(REQUEST_BODY_MAX_SIZE))
        std::cout <<  "write success" << std::endl;
}

void Server::readCGI() {
    uintptr_t client_fd;

    client_fd = cgi_fd[curr_event->ident];
    if (clients[client_fd].cgi_read(REQUEST_BODY_MAX_SIZE))
        std::cout <<  "read success" <<  cgi_fd[curr_event->ident] << std::endl;

	setResDefaultHeaderField(client_fd);
    change_events(client_fd, EVFILT_WRITE, EV_ENABLE);
}
