#include "Server.hpp"
#include "../config/Config.hpp"
#include "ContentType/ContentType.hpp"

/*
** server block 확인
*/
int Server::checkServerSocket(uintptr_t const & fd) const {
	int size = config.size();

	for (int i = 0; i < size; ++i) {
		if (fd == server_socket[i])
			return i;
	}
	return -1;
}

/*
** file fd인지 확인
*/
bool Server::checkFileFd() const {
	if (file_fd.find(curr_event->ident) != file_fd.end())
		return true;
	else
		return false;
}


/*
** cgi fd인지 확인
*/
bool Server::checkCGIFd() const {
	if (cgi_fd.find(curr_event->ident) != cgi_fd.end())
		return true;
	else
		return false;
}


/*
** connect client : add kqueue event & add fd_list
*/
void Server::change_events(uintptr_t const & ident, int16_t const & filter,
								uint16_t const & flags)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, 0, 0, NULL);
	change_list.push_back(temp_event);
}

/*
** disconnect client : close fd & erase fd at fd_list
*/
void Server::disconnect_client(uintptr_t fd) {
	std::cout << "client disconnected: " << fd << std::endl;

	close(fd);
	clients[fd].resetHTTP();
	clients.erase(fd);
}

/*
** disconnect file fd : close fd & erase fd at file_fd
*/
void Server::disconnect_file_fd() {
	change_events(file_fd[curr_event->ident], EVFILT_WRITE, EV_ENABLE);
	close(curr_event->ident);
	file_fd.erase(curr_event->ident);
}

/*
** timeout 확인하기
*/
void Server::checkClientTimeout() {
	std::vector<uintptr_t> fd(clients.size());
	int i = 0;

	// todo req 중이면 끝내지 않기
	for (std::map<uintptr_t, HTTP>::iterator it = clients.begin();
		it != clients.end(); ++it) {
			if (get_time() - it->second.getTimeOut() > TIME_OUT) {
				std::cout << get_time() - it->second.getTimeOut() << std::endl;
				fd[i++] = it->first;
			}
	}
	for (int j = 0; j < i; ++j) {
		std::cout << "time out! : " << fd[j] << std::endl;
		disconnect_client(fd[j]);
	}
}

/*
** check keep-alive
*/
void Server::checkKeepAlive() {
	if (clients[curr_event->ident].getKeepAlive() == false)
		disconnect_client(curr_event->ident);
}

/*
** 파일이 존재하는지 확인
*/
bool Server::existFile() {
	struct stat ss;
	std::string path = clients[curr_event->ident].getResponseFileDirectory();

	if (stat(path.c_str(), &ss) == -1 || S_ISDIR(ss.st_mode))
		return false;
	else
		return true;
}

/*
**
*/
bool Server::checkMaxBodySize() {
	int sb = clients[curr_event->ident].getResServerBlockIndex();
	int lb = clients[curr_event->ident].getResLocationIndex();

	if (clients[curr_event->ident].getBody().length() > static_cast<size_t>(config[sb].location[lb].client_max_body_size)) {
		clients[curr_event->ident].setStatus(413);
		return false;
	}
	return true;
}

/*
** read 해야 하는 파일에 아무 내용도 없을 경우 (file open 후에)
*/
bool Server::setReadFileEmpty(int const & fd) {
	int 	ret;
	struct	stat buf;

	ret = stat(clients[fd].getResponseFileDirectory().c_str(), &buf);

	if (buf.st_size != 0)
		return false;

	setResDefaultHeaderField(fd);
	clients[fd].setStatus(200);
	clients[fd].setResponseLine();
	if (isMethodHEAD(fd))
		return true;
	ContentType ct(clients[fd].getResponseFileDirectory());
	clients[fd].setResponseHeader("Content-Type", ct.getContentType());
	clients[fd].setResponseBody("");
	clients[fd].setResponseHeader("Content-Length", "0");
	return true;
}

/*
** read 해야 하는 파일에 아무 내용도 없을 경우 (file open 전에)
*/
bool Server::checkReadFileEmpty(int const & fd) {
	int 	ret;
	struct	stat buf;

	ret = stat(clients[fd].getResponseFileDirectory().c_str(), &buf);

	if (buf.st_size != 0)
		return false;

	setResDefaultHeaderField(fd);
	clients[fd].setStatus(200);
	clients[fd].setResponseLine();
	if (isMethodHEAD(fd))
		return true;
	ContentType ct(clients[fd].getResponseFileDirectory());
	clients[fd].setResponseHeader("Content-Type", ct.getContentType());
	clients[fd].setResponseBody("");
	clients[fd].setResponseHeader("Content-Length", "0");
	change_events(fd, EVFILT_WRITE, EV_ENABLE);
	return true;
}
