#ifndef Server_HPP
#define Server_HPP

#include <iostream>
#include <sys/socket.h>
// #include <sys/types.h>
#include <sys/event.h>
// #include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <map>
#include "HTTP.hpp"

#define SOCKET_LISTEN_BACKLOG   5
#define SOCKET_PORT             80
#define SOCKET_ADDR             INADDR_ANY
#define SOCKET_READ_BUF         16384
#define KQUEUE_EVENT_LIST_SIZE  1024

/*
** socket를 관리해주는 객체
*/
class Server {
private:
	uintptr_t						server_socket;
	sockaddr_in				    	server_addr;
	int						    	kq;
	struct kevent			    	event_list[KQUEUE_EVENT_LIST_SIZE];
	std::vector<struct kevent>		change_list;
    struct kevent*					curr_event;
	std::map<uintptr_t, HTTP>		clients_map;
	struct timespec                 kq_timeout;
public:
	Server();
	/* Server_socket */
	void socketInit();
	void socketRun();

	/* Server_kqueue */
	void kqueueInit();
	void kqueueEventRun();

	/* in kqueue eventRun */
	void kqueueEventError();
	void kqueueEventRead();
	void kqueueConnectAccept();
	void kqueueEventReadClient();
	void kqueueEventWrite();
	void change_events(uintptr_t const & ident, int16_t const & filter, uint16_t const & flags);
	void disconnect_client();
};

#endif
