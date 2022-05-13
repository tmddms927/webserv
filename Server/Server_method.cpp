#include "Server.hpp"
#include <stdio.h>
#include "ContentType/ContentType.hpp"

/*
** set OK response message - directory doesn't have index file
*/
void Server::setResOKMes() {
	setResDefaultHeaderField(curr_event->ident);
	clients[curr_event->ident].setResponseLine();

	if (isMethodHEAD(curr_event->ident) == false) {
		ContentType ct(clients[curr_event->ident].getResponseFileDirectory());
		clients[curr_event->ident].setResponseHeader("Content-Type", ct.getContentType());
		clients[curr_event->ident].setResponseBody("123");
		clients[curr_event->ident].setResponseHeader("Content-Length", "3");
	}
}

/*
** set error response message
*/
void Server::setResErrorMes(int const & client) {
	int fd;
	std::string file = config[clients[curr_event->ident].getResServerBlockIndex()].location[0].location_root\
			+ "/" + config[clients[curr_event->ident].getResServerBlockIndex()].location[0].err_page;

	fd = open(file.c_str(), O_RDONLY);
	if (fd < 0) {
		setResDefaultHeaderField(curr_event->ident);
		clients[client].setResponseLine();
	}
	else {
		file_fd[fd] = client;
		clients[curr_event->ident].setResponseHaveFileFd(true);
		change_events(fd, EVFILT_READ, EV_ADD | EV_ENABLE);
	}
}

/*
** set GET response message
*/
void Server::setResMethodGET() {
	int fd;

	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);

	if (fd <= 0)
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

	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
	int fd;

	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		changeStatusToError(curr_event->ident, 404);
	else {
		file_fd[fd] = curr_event->ident;
		clients[curr_event->ident].setResponseHaveFileFd(true);
		change_events(fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
	}
}

/*
** set DELETE response message
*/
void Server::setResMethodDELETE() {
	if (remove(clients[curr_event->ident].getResponseFileDirectory().c_str()) != 0)
		return changeStatusToError(curr_event->ident, 404);

	setResDefaultHeaderField(curr_event->ident);
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
	int fd;

	fd = file_fd[curr_event->ident];
	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 1);
	len = read(curr_event->ident, buf, RECIEVE_BODY_MAX_SIZE + 1);
	if (len <= RECIEVE_BODY_MAX_SIZE && len > 0 && !isMethodHEAD(fd)) {
		ContentType ct(clients[fd].getResponseFileDirectory());
		clients[fd].setResponseHeader("Content-Type", ct.getContentType());
		clients[fd].setResponseBody(buf);
		clients[fd].setResponseHeader("Content-Length", ft_itoa(len));
	}
	setResDefaultHeaderField(fd);
	clients[fd].setResponseLine();
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

	if (isMethodHEAD(fd) == false) {
		ContentType ct(clients[fd].getResponseFileDirectory());
		clients[fd].setResponseHeader("Content-Type", ct.getContentType());
		clients[fd].setResponseBody(buf);
		clients[fd].setResponseHeader("Content-Length", ft_itoa(len));
	}
	setResDefaultHeaderField(fd);
	clients[fd].setStatus(200);
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

	setResDefaultHeaderField(fd);
	clients[fd].setStatus(200);
	// post body 있어야되나..?
	// clients[fd].setResponseBody("");
	// clients[fd].setResponseHeader("Content-Length", ft_itoa(0));
	clients[fd].setResponseLine();
}

/*
** write POST file
*/
void Server::writeResPUTFile() {
	std::string req_body;
	size_t		len;
	int fd;

	fd = file_fd[curr_event->ident];
	req_body = clients[fd].getBody();
	len = write(curr_event->ident, req_body.c_str(), req_body.length());
	if (len != req_body.length())
		return changeStatusToError(fd, 404);

	setResDefaultHeaderField(fd);
	clients[fd].setStatus(200);
	// post body 있어야되나..?
	clients[fd].setResponseBody("good!");
	clients[fd].setResponseHeader("Content-Length", ft_itoa(5));
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
	setResDefaultHeaderField(fd);
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

	std::cout <<  clients[curr_event->ident].getResponseFileDirectory() << std::endl;
	std::cout << "[[[[ response message! ]]]]" << std::endl;
	std::cout << "[[[[" << message << "]]]]" << std::endl;

	/////////////////////////////////////////////////////

	write(curr_event->ident, message.c_str(), message.length());
}

/*
** input default response header field
*/
void Server::setResDefaultHeaderField(uintptr_t fd) {
	clients[fd].setResponseHeader("Server", SERVER_DEFAULT_NAME);
	clients[fd].setResponseHeader("Date", "Tue, 26 Apr 2022 10:59:45 GMT");
}

/*
** change status to error status
*/
void Server::changeStatusToError(int const & client, int const & st) {
	clients[client].setStatus(st);
	setResErrorMes(client);
}

/*make 
** method가 HEAD인지 확인
*/
bool Server::isMethodHEAD(uintptr_t fd) {
	char method = clients[fd].getMethod();

	if (method == HEAD_BIT)
		return true;
	else
		return false;
}
