#include "Server.hpp"

void Server::setError() {
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setGETHeader();
	clients[curr_event->ident].setErrorResponse();
}

void Server::setMethodGet() {
	int fd;

	// todo 파일인지 경로인지 확인 필요!
	// 경로면 서버의 인덱스 확인
	fd = open(clients[curr_event->ident].getURI().c_str(), O_RDONLY);
	if (fd < 0) {
		clients[curr_event->ident].setStatus(404);
		setError();
	}
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setResponseFd(static_cast<uintptr_t>(fd));
	clients[curr_event->ident].setGETHeader();
	std::cout << "get : file fd resister" << std::endl;
}

void Server::setMethodPost() {
	int fd;

	fd = open(clients[curr_event->ident].getURI().c_str(), O_RDONLY);
	if (fd < 0) {
		clients[curr_event->ident].setStatus(404);
		setError();
	}
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setResponseFd(fd);
	clients[curr_event->ident].setPOSTHeader();
	std::cout << "post : file fd resister" << std::endl;
}
