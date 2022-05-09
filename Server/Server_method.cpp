#include "Server.hpp"
#include <stdio.h>

/*
** find server block
** 아예 수정해야 됨!
*/
void Server::findServerBlock() {
	int size = config.size();
	size_t found;
	std::string uri = clients[curr_event->ident].getURI();

	// 처음에 / 없을 때
	found = uri.find("/");
    if (found == std::string::npos || found != 0) {
		clients[curr_event->ident].setStatus(404);
        return ;
	}

	// server block이 없을 경우
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
}

/*
** set error response message
*/
void Server::setResErrorMes(int const & client) {
	int fd;

	fd = open(global_config.err_page.c_str(), O_RDONLY);
	if (fd < 0) {
		setResDefaultHeaderField();
		clients[client].setResponseLine();
	}
	else {
		file_fd[fd] = client;
		clients[curr_event->ident].setResponseHaveFileFd(true);
		change_events(client, EVFILT_READ, EV_ADD | EV_ENABLE);
	}
}

/*
** set GET response message
*/
void Server::setResMethodGET() {
	int fd;

	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);
	if (fd < 0)
		changeStatusToError(curr_event->ident, 404);
	else {
		file_fd[fd] = curr_event->ident;
		clients[curr_event->ident].setResponseHaveFileFd(true);
		change_events(fd, EVFILT_READ, EV_ADD | EV_ENABLE);
	}
}

/*
** set POST response message
*/
void Server::setResMethodPOST() {
	int fd;

	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);
	if (fd < 0)
		changeStatusToError(curr_event->ident, 404);
	else {
		file_fd[fd] = curr_event->ident;
		clients[curr_event->ident].setResponseHaveFileFd(true);
		change_events(fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
	}
}

/*
** set PUT response message
*/
void Server::setResMethodPUT() {
	setResDefaultHeaderField();
	clients[curr_event->ident].setStatus(200);
	clients[curr_event->ident].setResponseBody("");
	clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(0));
	clients[curr_event->ident].setResponseLine();
}

/*
** set DELETE response message
*/
void Server::setResMethodDELETE() {
	if (remove(clients[curr_event->ident].getResponseFileDirectory().c_str()) != 0)
		return changeStatusToError(curr_event->ident, 404);

	setResDefaultHeaderField();
	clients[curr_event->ident].setStatus(200);
	clients[curr_event->ident].setResponseLine();
}

/*
** set HEAD response message
*/
void Server::setResMethodHEAD() {
	int fd;

	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);
	if (fd < 0)
		changeStatusToError(curr_event->ident, 404);
	else {
		file_fd[fd] = curr_event->ident;
		clients[curr_event->ident].setResponseHaveFileFd(true);
		change_events(fd, EVFILT_READ, EV_ADD | EV_ENABLE);
	}
}

/*
** read ERROR file
*/
void Server::readResErrorFile() {
	char buf[RECIEVE_BODY_MAX_SIZE + 1];
	size_t len;

	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 1);
	len = read(curr_event->ident, buf, RECIEVE_BODY_MAX_SIZE + 1);
	if (len <= RECIEVE_BODY_MAX_SIZE && len > 0) {
		clients[curr_event->ident].setResponseBody(buf);
		clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(len));
	}
	setResDefaultHeaderField();
	clients[curr_event->ident].setResponseLine();
}

/*
** read GET file
*/
void Server::readResGETFile() {
	char buf[RECIEVE_BODY_MAX_SIZE + 1];
	size_t len;
	int fd;

	fd = file_fd[curr_event->ident];
	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 1);
	len = read(curr_event->ident, buf, RECIEVE_BODY_MAX_SIZE + 1);
	if (len > RECIEVE_BODY_MAX_SIZE)
		return changeStatusToError(fd, 404);
	else if (len < 0)
		return changeStatusToError(fd, 500);

	setResDefaultHeaderField();
	clients[fd].setStatus(200);
	clients[fd].setResponseBody(buf);
	clients[fd].setResponseHeader("Content-Length", ft_itoa(len));
	clients[fd].setResponseLine();
}

/*
** write POST file
*/
void Server::writeResPOSTFile() {
	std::string req_body;
	size_t		len;
	int fd;

	fd = file_fd[curr_event->ident];
	req_body = clients[fd].getBody();
	len = write(curr_event->ident, req_body.c_str(), req_body.length());
	if (len != req_body.length())
		return changeStatusToError(fd, 404);

	setResDefaultHeaderField();
	clients[fd].setStatus(200);
	// post body 있어야되나..?
	clients[fd].setResponseBody("");
	clients[fd].setResponseHeader("Content-Length", ft_itoa(0));
	clients[fd].setResponseLine();
}

/*
** read HEAD file
*/
void Server::readResHEADFile() {
	char buf[RECIEVE_BODY_MAX_SIZE + 1];
	size_t len;
	int fd;

	fd = file_fd[curr_event->ident];
	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 1);
	len = read(curr_event->ident, buf, RECIEVE_BODY_MAX_SIZE + 1);
	if (len > RECIEVE_BODY_MAX_SIZE)
		return changeStatusToError(fd, 404);
	else if (len < 0)
		return changeStatusToError(fd, 500);

	clients[fd].setStatus(200);
	setResDefaultHeaderField();
	clients[fd].setResponseLine();
}

/*
** send response message to client
*/
void Server::sendResMessage() {
	std::string message;

	message = clients[curr_event->ident].getResponseLine();
	message += "\r\n";
	message += clients[curr_event->ident].getResponseHeader();
	message += "\r\n";
	message += clients[curr_event->ident].getResponseBody();

	/////////////////////////////////////////////////////
	std::cout << "==========================" << std::endl;
	// std::cout << clients[curr_event->ident].getResponseFileDirectory() << std::endl;
	// std::cout << "[[[[ request message! ]]]]" << std::endl;
	// clients[curr_event->ident].reqPrint();
	std::cout << "[[[[ response message! ]]]]" << std::endl;
	std::cout << "[[[[" << message << "]]]]" << std::endl;
	/////////////////////////////////////////////////////

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
void Server::changeStatusToError(int const & client, int const & st) {
	clients[client].setStatus(st);
	setResErrorMes(client);
}

/*
** directory이면 index file 붙여주기
*/
void Server::isFile() {
	std::string path = clients[curr_event->ident].getResponseFileDirectory();
	struct stat ss;

	if (stat(path.c_str(), &ss) == -1)
		return changeStatusToError(curr_event->ident, 401);
	if (S_ISDIR(ss.st_mode))
		clients[curr_event->ident].setResponseFileDirectory(path + global_config.index);
}
