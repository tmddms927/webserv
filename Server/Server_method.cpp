#include "Server.hpp"
#include <stdio.h>
#include "ContentType/ContentType.hpp"
#include "../autoindex/AutoIndex.hpp"

/*
** set error response message
*/
void Server::setResErrorMes(int const & client) {
	int fd;
	int block = clients[curr_event->ident].getResServerBlockIndex();
	int location = clients[curr_event->ident].getResLocationIndex();

	std::string file = config[block].location[location].location_root\
			+ "/" + config[block].location[location].err_page;

	clients[curr_event->ident].setResponseFileDirectory(file);
	if (checkReadFileEmpty(curr_event->ident))
		return ;
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

	if (checkReadFileEmpty(curr_event->ident))
		return ;
	fd = open(clients[curr_event->ident].getResponseFileDirectory().c_str(), O_RDONLY);
	if (fd <= 0)
		checkAutoIndex();
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
	clients[curr_event->ident].setStatus(204);
	setResDefaultHeaderField(curr_event->ident);
	clients[curr_event->ident].setResponseLine();
}

/*
** set PUT response message
*/
void Server::setResMethodPUT() {
	int fd;

	if (existFile()) {
		clients[curr_event->ident].setStatus(204);
		setResDefaultHeaderField(curr_event->ident);
		clients[curr_event->ident].setResponseLine();
		return ;
	}
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
	std::string file = clients[curr_event->ident].getResponseFileDirectory();
	if (remove(file.c_str()) != 0)
		return changeStatusToError(curr_event->ident, 404);

	setResDefaultHeaderField(curr_event->ident);
	clients[curr_event->ident].setStatus(200);
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setResponseHeader("Content-Type", "text/plain");
	clients[curr_event->ident].setResponseBody(file);
	clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(file.length()));
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
	int len;
	int fd;

	fd = file_fd[curr_event->ident];
	if (setReadFileEmpty(fd))
		return ;
	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 1);
	len = read(curr_event->ident, buf, RECIEVE_BODY_MAX_SIZE + 1);
	if (setReadFileEmpty(fd) || len == 0 || len == -1) {
		setResDefaultHeaderField(fd);
		clients[fd].setResponseLine();
		return ;
	}
	if (len <= RECIEVE_BODY_MAX_SIZE && len > 0 && !isMethodHEAD(fd)) {
		ContentType ct(clients[fd].getResponseFileDirectory());
		clients[fd].setResponseHeader("Content-Type", ct.getContentType());
		clients[fd].setResponseBody(std::string(buf, len));
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
	int len;
	int fd;

	fd = file_fd[curr_event->ident];
	if (setReadFileEmpty(fd))
		return ;
	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 1);
	len = read(curr_event->ident, buf, RECIEVE_BODY_MAX_SIZE + 1);
	if (setReadFileEmpty(fd) || len == 0) {
		clients[fd].setStatus(500);
		setResDefaultHeaderField(fd);
		clients[fd].setResponseLine();
		return ;
	}
	if (len > RECIEVE_BODY_MAX_SIZE)
		return changeStatusToError(fd, 404);
	else if (len == -1) {
		clients[fd].setResponseBody("");
		clients[fd].setResponseHeader("Content-Length", "0");
	}
	else {
		clients[fd].setResponseBody(std::string(buf, len));
		clients[fd].setResponseHeader("Content-Length", ft_itoa(len));
	}
	ContentType ct(clients[fd].getResponseFileDirectory());
	clients[fd].setResponseHeader("Content-Type", ct.getContentType());

	setResDefaultHeaderField(fd);
	clients[fd].setStatus(200);
	clients[fd].setResponseLine();
}

/*
** write POST file
*/
void Server::writeResPUTFile() {
	std::string req_body;
	int			len;
	int			fd;

	fd = file_fd[curr_event->ident];
	req_body = clients[fd].getBody();
	len = write(curr_event->ident, req_body.c_str(), req_body.length());
	if (len != static_cast<int>(req_body.length()) || len == -1 || len == 0)
		return changeStatusToError(fd, 404);

	std::string file = clients[fd].getResponseFileDirectory();
	setResDefaultHeaderField(fd);
	clients[fd].setStatus(201);
	clients[fd].setResponseLine();
	clients[fd].setResponseHeader("Content-Type", "text/plain");
	clients[fd].setResponseBody(file);
	clients[fd].setResponseHeader("Content-Length", ft_itoa(file.length()));
}

/*
** read HEAD file
*/
void Server::readResHEADFile() {
	char	buf[RECIEVE_BODY_MAX_SIZE + 1];
	int		len;
	int		fd;

	fd = file_fd[curr_event->ident];
	std::memset(buf, 0, RECIEVE_BODY_MAX_SIZE + 1);
	len = read(curr_event->ident, buf, RECIEVE_BODY_MAX_SIZE + 1);
	if (setReadFileEmpty(fd) || len == 0) {
		clients[fd].setStatus(500);
		setResDefaultHeaderField(fd);
		clients[fd].setResponseLine();
		return ;
	}
	if (len > RECIEVE_BODY_MAX_SIZE)
		return changeStatusToError(fd, 404);
	else if (len == -1)
		clients[fd].setStatus(200);
	else
		clients[fd].setStatus(200);
	setResDefaultHeaderField(fd);
	clients[fd].setResponseLine();
}

/*
** send response message to client
*/
void Server::sendResMessage() {
	if (clients[curr_event->ident].getResponseStep() == CLIENT_RES_LINE)
		sendResLine();
	if (clients[curr_event->ident].getResponseStep() == CLIENT_RES_HEADER)
		sendResHeader();
	if (clients[curr_event->ident].getResponseStep() == CLIENT_RES_BODY)
		sendResBody();
	if (clients[curr_event->ident].getResponseStep() == CLIENT_RES_FINISH) {
		change_events(curr_event->ident, EVFILT_WRITE, EV_DISABLE);
		change_events(curr_event->ident, EVFILT_READ, EV_ENABLE);
		checkKeepAlive();
		clients[curr_event->ident].resetHTTP();
	}
}

/*
** check redirect
*/
bool Server::checkRedirect() {
	int sb = clients[curr_event->ident].getResServerBlockIndex();
	int lb = clients[curr_event->ident].getResLocationIndex();

	if (config[sb].location[lb].redirect_code == -1)
		return false;

	clients[curr_event->ident].resetResponseHeader();
	clients[curr_event->ident].resetResponseBody();

	setResDefaultHeaderField(curr_event->ident);
	clients[curr_event->ident].setRedirectStatus(config[sb].location[lb].redirect_code);
	clients[curr_event->ident].setResponseLine();
	clients[curr_event->ident].setResponseHeader("Location", config[sb].location[lb].redirect_uri);
	return true;
}

/*
** send response line to client
*/
void Server::sendResLine() {
	int		length = 0;
	size_t	index = clients[curr_event->ident].getResponseIndex();

	if (index == 0)
		clients[curr_event->ident].setResponseHeaderFinish();
	length = write(curr_event->ident, clients[curr_event->ident].getResponseLine().c_str() + index,
		clients[curr_event->ident].getResponseLine().length() - index);
	if (length == 0)
		return ;
	if (length == -1)
		return disconnect_client(curr_event->ident);
	if (index + length != clients[curr_event->ident].getResponseLine().length())
		clients[curr_event->ident].setResponseIndex(index + length);
	else {
		clients[curr_event->ident].setResponseIndex(0);
		clients[curr_event->ident].setResponseStep(CLIENT_RES_HEADER);
	}
}

/*
** send response header to client
*/
void Server::sendResHeader() {
	int		length = 0;
	size_t	index = clients[curr_event->ident].getResponseIndex();

	length = write(curr_event->ident, clients[curr_event->ident].getResponseHeader().c_str() + index,
		clients[curr_event->ident].getResponseHeader().length() - index);
	if (length == 0)
		return ;
	if (length == -1)
		return disconnect_client(curr_event->ident);
	if (index + length != clients[curr_event->ident].getResponseHeader().length())
		clients[curr_event->ident].setResponseIndex(index + length);
	else {
		clients[curr_event->ident].setResponseIndex(0);
		clients[curr_event->ident].setResponseStep(CLIENT_RES_BODY);
	}
}

/*
** send response body to client
*/
void Server::sendResBody() {
	int		length = 0;
	size_t	index = clients[curr_event->ident].getResponseIndex();

	if (clients[curr_event->ident].getResponseBody().length() == 0) {
		clients[curr_event->ident].setResponseIndex(0);
		clients[curr_event->ident].setResponseStep(CLIENT_RES_FINISH);
		return ;
	}
	length = write(curr_event->ident, clients[curr_event->ident].getResponseBody().c_str() + index,
		clients[curr_event->ident].getResponseBody().length() - index);
	if (length == 0)
		return ;
	if (length == -1)
		return disconnect_client(curr_event->ident);
	if (index + length != clients[curr_event->ident].getResponseBody().length())
		clients[curr_event->ident].setResponseIndex(index + length);
	else {
		clients[curr_event->ident].setResponseIndex(0);
		clients[curr_event->ident].setResponseStep(CLIENT_RES_FINISH);
	}
}

/*
** input default response header field
*/
void Server::setResDefaultHeaderField(uintptr_t fd) {
	clients[fd].setResponseHeader("Server", config[clients[fd].getResServerBlockIndex()].host);
}

/*
** change status to error status
*/
void Server::changeStatusToError(int const & client, int const & st) {
	clients[client].setStatus(st);
	setResErrorMes(client);
}

/*
** method가 HEAD인지 확인
*/
bool Server::isMethodHEAD(uintptr_t fd) {
	char method = clients[fd].getMethod();

	if (method == HEAD_BIT)
		return true;
	else
		return false;
}

void Server::checkAutoIndex() {
	bool err;
	std::string body;

	if (config[clients[curr_event->ident].getResServerBlockIndex()].\
		location[clients[curr_event->ident].getResLocationIndex()].auto_index != 1) {
		return changeStatusToError(curr_event->ident, 404);
	}

	AutoIndex autoIndex(config[clients[curr_event->ident].getResServerBlockIndex()].\
		location[clients[curr_event->ident].getResLocationIndex()].location_root);
	err = autoIndex.makeHTML();
	if (err) {
		return changeStatusToError(curr_event->ident, 404);
	}
	else {
		body = autoIndex.getRes().body;
		clients[curr_event->ident].setStatus(200);
		clients[curr_event->ident].setResponseBody(body);
		clients[curr_event->ident].setResponseHeader("Content-Length", ft_itoa(body.length()));
		clients[curr_event->ident].setResponseLine();
		setResDefaultHeaderField(curr_event->ident);
	}
}
