#include "Server.hpp"

/*
** write to cgi
*/
void Server::writeCGI() {
	uintptr_t client_fd;
	int status;

	client_fd = cgi_fd[curr_event->ident];
	status = clients[client_fd].cgi_write();
	if (status == CGI_FINISHED)
		cgi_fd.erase(curr_event->ident);
	if (status == CGI_ERROR) {
		setResDefaultHeaderField(client_fd);
		clients[client_fd].setStatus(500);
		clients[client_fd].setResponseLine();
		change_events(client_fd, EVFILT_WRITE, EV_ENABLE);
		cgi_fd.erase(curr_event->ident);
		cgi_fd.erase(clients[client_fd].getResponseCGIReadFd());
		close(clients[client_fd].getResponseCGIReadFd());
		close(curr_event->ident);
	}
}

void Server::readCGI() {
	uintptr_t client_fd;
	int status;

	client_fd = cgi_fd[curr_event->ident];
	status = clients[client_fd].cgi_read();
	if (status == CGI_ERROR) {
		setResDefaultHeaderField(client_fd);
		clients[client_fd].setStatus(500);
		clients[client_fd].setResponseLine();
		change_events(client_fd, EVFILT_WRITE, EV_ENABLE);
		cgi_fd.erase(curr_event->ident);
		close(curr_event->ident);
	}
	if (status == CGI_FINISHED) {
		if (clients[client_fd].cgi_setResponseline() == CGI_ERROR) {
			clients[client_fd].resetResponseBody();
			clients[client_fd].resetResponseHeader();
			clients[client_fd].setRedirectStatus(500);
			clients[client_fd].setResponseLine();
		}
		if (clients[client_fd].cgi_setResponseHeader() == CGI_ERROR) {
			clients[client_fd].resetResponseBody();
			clients[client_fd].resetResponseHeader();
			clients[client_fd].setRedirectStatus(500);
			clients[client_fd].setResponseLine();
		}
		setResDefaultHeaderField(client_fd);
		change_events(client_fd, EVFILT_WRITE, EV_ENABLE);
		cgi_fd.erase(curr_event->ident);
		wait(NULL);
	}
}
