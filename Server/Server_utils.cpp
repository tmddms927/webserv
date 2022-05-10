#include "Server.hpp"
#include "../config/Config.hpp"

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
	long current_time = get_time();
	std::vector<uintptr_t> fd(clients.size());
	int i = 0;

	// todo req 중이면 끝내지 않기
	for (std::map<uintptr_t, HTTP>::iterator it = clients.begin();
		it != clients.end(); ++it) {
			if (current_time - it->second.getReqFinishedTime() > TIME_OUT)
				fd[i++] = it->first;
	}
	for (int j = 0; j < i; ++j)
		disconnect_client(fd[j]);
}

/*
** allowed method 확인하기
*/
void Server::checkAllowedMethod() {
	// todo char a -> method 수정하기!!!!!!!!!!!!!!!!!!!!!!!!!!!
	char a = PUT_BIT;

	if ((global_config.allowed_method & a) == GET_BIT)
		return ;
	if ((global_config.allowed_method & a) == POST_BIT)
		return ;
	if ((global_config.allowed_method & a) == DELETE_BIT)
		return ;
	if ((global_config.allowed_method & a) == PUT_BIT)
		return ;
	if ((global_config.allowed_method & a) == HEAD_BIT)
		return ;
	changeStatusToError(curr_event->ident, 404);
}

/*
** check keep-alive
*/
void Server::checkKeepAlive() {
	if (clients[curr_event->ident].getKeepAlive() == false)
		disconnect_client(curr_event->ident);
}
