#include "Server.hpp"

void Server::findServerBlock() {
	int size = config.size();

	for (int i = 1; i < size; ++i) {
		if (server_socket[i] == clients[curr_event->ident].getServerFd()) {
			clients[curr_event->ident].setResponseFileDirectory(config[i].location + clients[curr_event->ident].getURI());
			return ;
		}
	}
	clients[curr_event->ident].setResponseFileDirectory(clients[curr_event->ident].getURI());
}

void Server::setError() {
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setGETHeader();
	clients[curr_event->ident].setErrorResponse();
}

void Server::setMethodGet() {
	int fd;
	char buf[RECIEVE_BODY_MAX_SIZE + 2];
	int len;

	// todo 파일인지 경로인지 확인 필요!
	// 경로면 서버의 인덱스 확인
	fd = open(clients[curr_event->ident].getURI().c_str(), O_RDONLY);
	if (fd < 0) {
		clients[curr_event->ident].setStatus(404);
		setError();
	}

	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 2);
	len = read(fd, buf, RECIEVE_BODY_MAX_SIZE + 2);
	if (len > RECIEVE_BODY_MAX_SIZE) {
		// 내용물이 너무 큼
		clients[curr_event->ident].setStatus(404);
		setError();
	}

	buf[0] = 'h';
	buf[1] = 'i';

	clients[curr_event->ident].setResponseBody(buf);
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setResponseFd(static_cast<uintptr_t>(fd));
	clients[curr_event->ident].setGETHeader();
	std::cout << "get : file fd resister" << std::endl;
}

void Server::setMethodPost() {
	int fd;
	int len;
	std::string req_body;

	// todo 파일인지 경로인지 확인 필요!
	// 경로면 서버의 인덱스 확인
	fd = open(clients[curr_event->ident].getURI().c_str(), O_RDONLY);
	if (fd < 0) {
		clients[curr_event->ident].setStatus(404);
		setError();
	}

	req_body = clients[curr_event->ident].getBody();
	write(fd, req_body.c_str(), req_body.length());
	clients[curr_event->ident].setResponseBody("");
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setResponseFd(static_cast<uintptr_t>(fd));
	clients[curr_event->ident].setGETHeader();
	std::cout << "get : file fd resister" << std::endl;
}

void Server::resSendMessage() {
	std::string message;

	message = clients[curr_event->ident].getResponseLine();
	message += "\r\n";
	message += clients[curr_event->ident].getResponseHeader();
	message += "\r\n";
	message += clients[curr_event->ident].getResponseBody();
	message += "\r\n\r\n";
	std::cout << "[" << message << "]" << std::endl;
	write(curr_event->ident, message.c_str(), message.length());
	std::cout << "hahahahah!" << std::endl;
}
