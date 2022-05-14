#include "Server.hpp"

/*
** write to cgi
*/
void Server::writeCGI() {
	uintptr_t client_fd;
	int status;

	client_fd = cgi_fd[curr_event->ident];
	status = clients[client_fd].cgi_write(REQUEST_BODY_MAX_SIZE);

	// if (status == CGI_WRITE_FINSHED) {
	// 	// write 끝 read event 추가
	// 	std::cout <<  "write success" << std::endl;
	// }
	if (status == CGI_ERROR)
		; // pipe의 이상이 생겼을 때 체크추가
}

void Server::readCGI() {
	uintptr_t client_fd;
	int status;

	client_fd = cgi_fd[curr_event->ident];
	status = clients[client_fd].cgi_read(REQUEST_BODY_MAX_SIZE);
	if (status == CGI_FINISHED) {
		setResDefaultHeaderField(client_fd);
		change_events(client_fd, EVFILT_WRITE, EV_ENABLE);
		std::cout <<  "read success" <<  cgi_fd[curr_event->ident] << std::endl;
	}

}
