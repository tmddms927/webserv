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
	if (status == CGI_ERROR)
		; // pipe의 이상이 생겼을 때 체크추가
}

void Server::readCGI() {
	uintptr_t client_fd;
	int status;

	client_fd = cgi_fd[curr_event->ident];
	status = clients[client_fd].cgi_read();
	if (status == CGI_ERROR)
		; // pipe의 이상이 생겼을 때 체크추가
	if (status == CGI_FINISHED) {
		if (clients[client_fd].cgi_setResponseline() == CGI_FINISHED)
			; // error 처리
		if (clients[client_fd].cgi_setResponseHeader() == CGI_FINISHED)
			; // error 처리
		setResDefaultHeaderField(client_fd);
		change_events(client_fd, EVFILT_WRITE, EV_ENABLE);
		cgi_fd.erase(curr_event->ident);
	}
}
