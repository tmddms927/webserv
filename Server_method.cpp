#include "Server.hpp"

void Server::findServerBlock() {
	int size = config.size();
	size_t found;
	std::string uri = clients[curr_event->ident].getURI();

	found = uri.find("/");
    if (found == std::string::npos || found != 0) {
		clients[curr_event->ident].setStatus(404);
        return ;
	}

    std::string temp = uri.substr(1);
	found = temp.find("/");
    if (found == std::string::npos) {
		clients[curr_event->ident].setResponseFileDirectory(config[0].root + uri);
		isFile();
        return ;
	}
	temp = uri.substr(0, found + 1);

	for (int i = 1; i < size; ++i) {
		if (server_socket[i] == clients[curr_event->ident].getServerFd()) {
			if (temp == config[i].location) {
				clients[curr_event->ident].setResponseFileDirectory(config[i].root + uri);
				isFile();
				return ;
			}
		}
	}
	// isFile();
}

/*
** set error response message
*/
void Server::setResErrorMes() {
	int fd;
	char buf[RECIEVE_BODY_MAX_SIZE];
	int len;

	fd = open(global_config.err_page.c_str(), O_RDONLY);
	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE);
	len = read(fd, buf, RECIEVE_BODY_MAX_SIZE);

	setResDefaultHeaderField();
	clients[curr_event->ident].setResponseBody(buf);
	clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(len));
	clients[curr_event->ident].setResponseLine();
}

void Server::setResMethodGET() {
	int fd;
	char buf[RECIEVE_BODY_MAX_SIZE + 2];
	int len;

	// todo 파일인지 경로인지 확인 필요!
	// 경로면 서버의 인덱스 확인
	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);
	if (fd < 0)
		return changeStatusToError(404);
	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 2);
	len = read(fd, buf, RECIEVE_BODY_MAX_SIZE + 2);
	if (len > RECIEVE_BODY_MAX_SIZE)
		return changeStatusToError(404);
	else if (len < 0)
		return changeStatusToError(500);

	setResDefaultHeaderField();
	clients[curr_event->ident].setStatus(200);
	clients[curr_event->ident].setResponseBody(buf);
	clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(len));
	clients[curr_event->ident].setResponseLine();
}

void Server::setResMethodPOST() {
	int fd;
	std::string req_body;

	// todo 파일인지 경로인지 확인 필요!
	// 경로면 서버의 인덱스 확인
	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);
	if (fd < 0)
		return changeStatusToError(404);
	req_body = clients[curr_event->ident].getBody();
	write(fd, req_body.c_str(), req_body.length());

	// post body 있어야되나..?
	setResDefaultHeaderField();
	clients[curr_event->ident].setStatus(200);
	clients[curr_event->ident].setResponseBody("");
	clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(0));
	clients[curr_event->ident].setResponseLine();
}

void Server::setResMethodPUT() {
	setResDefaultHeaderField();
	clients[curr_event->ident].setStatus(200);
	clients[curr_event->ident].setResponseBody("");
	clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(0));
	clients[curr_event->ident].setResponseLine();
}

void Server::setResMethodDELETE() {
	int fd;
	std::string req_body;

	// todo 파일인지 경로인지 확인 필요!
	// 경로면 서버의 인덱스 확인
	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);
	if (fd < 0)
		return changeStatusToError(404);
	
	// post body 있어야되나..?
	setResDefaultHeaderField();
	clients[curr_event->ident].setStatus(405);
	clients[curr_event->ident].setResponseBody("");
	clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(0));
	clients[curr_event->ident].setResponseLine();
}

void Server::setResMethodHEAD() {
	int fd;
	char buf[RECIEVE_BODY_MAX_SIZE + 2];
	int len;

/////////
return changeStatusToError(405);
/////////

	// todo 파일인지 경로인지 확인 필요!
	// 경로면 서버의 인덱스 확인
	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);
	if (fd < 0)
		return changeStatusToError(404);

	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 2);
	len = read(fd, buf, RECIEVE_BODY_MAX_SIZE + 2);
	if (len > RECIEVE_BODY_MAX_SIZE)
		return changeStatusToError(404);

	setResDefaultHeaderField();
	clients[curr_event->ident].setStatus(200);
	clients[curr_event->ident].setResponseLine();
}

void Server::sendResMessage() {
	std::string message;

	message = clients[curr_event->ident].getResponseLine();
	message += "\r\n";
	message += clients[curr_event->ident].getResponseHeader();
	message += "\r\n";
	message += clients[curr_event->ident].getResponseBody();
	std::cout << "[[[[ response message! ]]]]" << std::endl;
	std::cout << "[[[[" << message << "]]]]" << std::endl;
	write(curr_event->ident, message.c_str(), message.length());
}

/*
** input default response header field
*/
void Server::setResDefaultHeaderField() {
	std::string str;

	clients[curr_event->ident].setResponseHeader("Server", SERVER_DEFAULT_NAME);
	clients[curr_event->ident].setResponseHeader("Date", "Tue, 26 Apr 2022 10:59:45 GMT");
}

/*
** change status to error status
*/
void Server::changeStatusToError(int st) {
	clients[curr_event->ident].setStatus(st);
	setResErrorMes();
}


void Server::isFile() {
	std::string path = clients[curr_event->ident].getResponseFileDirectory();
	struct stat ss;

	if (stat(path.c_str(), &ss) == -1) {
		return changeStatusToError(404);
	}
	// directory
	if (S_ISDIR(ss.st_mode)) {
		// global_config.index = "index"
		clients[curr_event->ident].setResponseFileDirectory(path + "index");
	}
}
