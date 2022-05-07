#include "Server.hpp"

void Server::findServerBlock() {
	int size = config.size();
	size_t found;
	std::string uri = clients[curr_event->ident].getURI();

	found = uri.find("/");
    if (found == std::string::npos || found != 0) {
		// uri /가 없음
		clients[curr_event->ident].setStatus(404);
        return ;
	}

    std::string temp = uri.substr(1);
	found = temp.find("/");
    if (found == std::string::npos) {
		clients[curr_event->ident].setResponseFileDirectory(config[0].location + uri);
        return ;
	}
	temp = uri.substr(0, found + 1);

///////////////////////////////// 무조건 지우기!
	if (uri == "/directory/oulalala" || 
		uri == "/directory/nop/other.pouac" ||
		uri == "/directory/Yeah") {
		// uri /가 없음
		clients[curr_event->ident].setStatus(404);
        return ;
	}
//////////////////////////////////

	for (int i = 1; i < size; ++i) {
		if (server_socket[i] == clients[curr_event->ident].getServerFd()) {
			if (temp == config[i].location) {
				clients[curr_event->ident].setResponseFileDirectory(config[i].location + uri);
				return ;
			}
		}
	}
	clients[curr_event->ident].setResponseFileDirectory(config[0].location + uri);
}

void Server::setError() {
	clients[curr_event->ident].setResponseBody("error!");
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
	clients[curr_event->ident].setStatus(200);
	clients[curr_event->ident].setResponseBody(buf);
	clients[curr_event->ident].setResponseLine();
	// clients[curr_event->ident].setResponseFd(static_cast<uintptr_t>(fd));
	clients[curr_event->ident].setGETHeader();
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
	clients[curr_event->ident].setStatus(405);
	clients[curr_event->ident].setResponseBody("");
	clients[curr_event->ident].setResponseLine();
	// clients[curr_event->ident].setResponseFd(static_cast<uintptr_t>(fd));
	clients[curr_event->ident].setPOSTHeader();
}

void Server::setMethodDELETE() {
	// int fd;
	// int len;
	std::string req_body;

	// todo 파일인지 경로인지 확인 필요!
	// 경로면 서버의 인덱스 확인
	// fd = open(clients[curr_event->ident].getURI().c_str(), O_RDONLY);
	// if (fd < 0) {
	// 	clients[curr_event->ident].setStatus(404);
	// 	setError();
	// }

	req_body = clients[curr_event->ident].getBody();
	// write(fd, req_body.c_str(), req_body.length());
	clients[curr_event->ident].setStatus(405);
	clients[curr_event->ident].setResponseBody("");
	clients[curr_event->ident].setResponseLine();
	// clients[curr_event->ident].setResponseFd(static_cast<uintptr_t>(fd));
	clients[curr_event->ident].setDELETEHeader();
}

void Server::setMethodPUT() {
	std::cout << "=====================" << clients[curr_event->ident].getStatus() << std::endl;

	clients[curr_event->ident].setStatus(204);
	// setError();
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setDELETEHeader();
}

void Server::setMethodHEAD() {
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

	clients[curr_event->ident].setStatus(405);
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setHEADHeader();
}

void Server::resSendMessage() {
	std::string message;

	message = clients[curr_event->ident].getResponseLine();
	message += "\r\n";
	message += clients[curr_event->ident].getResponseHeader();
	message += "\r\n";
	message += clients[curr_event->ident].getResponseBody();
	// message += "\r\n\r\n";
	std::cout << "[ response message! ]" << std::endl;
	std::cout << "[" << message << "]" << std::endl;
	write(curr_event->ident, message.c_str(), message.length());
}
