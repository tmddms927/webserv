#ifndef SOCKETCONTROLLER_HPP
#define SOCKETCONTROLLER_HPP

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

#define SOCKET_LISTEN_BACKLOG 5
#define SOCKET_PORT 80
#define SOCKET_ADDR INADDR_ANY
#define SOCKET_READ_BUF    1024
#define KQUEUE_EVENT_LIST_SIZE   10

/*
** socket를 관리해주는 객체
*/
class SocketController {
private:
	int						    	server_socket;
	sockaddr_in				    	server_addr;
	int						    	kq;
    int                             sock_opt;
    struct kevent			    	event_list[8]; // kevent array for eventlist
    struct kevent*                  curr_event;
	std::vector<struct kevent>  	change_list;
    std::map<uintptr_t, HTTP>		fd_list;
    struct timespec                 timeout;
public:
	SocketController();
	void socketInit();
    void socketRun();
	void kqueueInit();
    void kqueueEventRun();
    /* in eventRun */
    void kqueueEventError();
    void kqueueEventRead();
    void kqueueEventWrite();

    /* in kqueueEventRead */
    void kqueueConnectAccept();
    void kqueueEventReadClient();

    void  change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	void disconnect_client(int client_fd);
    void sendMessage(uintptr_t fd);
};

#endif
