#ifndef Server_HPP
#define Server_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include "../HTTP.hpp"
#include "../config/Config.hpp"
#include "../utils.hpp"
#include "Server_define.hpp"

/*
** socket를 관리해주는 객체
*/
class Server {
private:
	const std::vector<servers>		config;
	const global					global_config;
	std::vector<uintptr_t>			server_socket;
	sockaddr_in						server_addr;
	int								kq;
	struct kevent					event_list[KQUEUE_EVENT_LIST_SIZE];
	struct kevent*					curr_event;
	std::map<uintptr_t, HTTP>		clients;
	struct timespec					kq_timeout;
	std::vector<struct kevent>		change_list;

public:
	/* Server.cpp */
	Server(Config const & c);

	/* Server_socket.cpp */
	void socketRun();
	void ServerSocketInit();
	void socketInit(int const & i);
	void removeBindError(int const & fd);
	void setSockaddr_in(int const & i);
	uintptr_t checkPort(int const & i, int const & port) const;

	/* Server_kqueue.cpp */
	void kqueueInit();
	void addServerSocketKevent();
	void kqueueEventRun();
	void checkKeventFilter();
	void kqueueEventError();
	void kqueueEventRead();
	void kqueueConnectAccept();
	void kqueueEventReadClient();
	void finishedRead();
	void checkMethod();
	void kqueueEventWrite();
	void change_events(uintptr_t const & ident,
			int16_t const & filter, uint16_t const & flags);
	void disconnect_client();
	int checkServerSocket(uintptr_t const & fd);

	/* Server_method */
    void findServerBlock();

	void setResErrorMes();
	void setResMethodGET();
	void setResMethodPOST();
	void setResMethodPUT();
	void setResMethodDELETE();
	void setResMethodHEAD();
	void sendResMessage();
	void setResDefaultHeaderField();
	void changeStatusToError(int const & st);
	void isFile(); 
};

#endif
