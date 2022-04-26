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
#include "RequestMessage.hpp"
//#include "ResponseMessage.hpp"

#define SOCKET_LISTEN_BACKLOG 5
#define SOCKET_PORT 80
#define SOCKET_ADDR INADDR_ANY

/*
** socket를 관리해주는 객체
*/
class SocketController {
private:
	int						    	server_socket;
	sockaddr_in				    	server_addr;
	int						    	kq;
    struct kevent			    	event_list[8]; // kevent array for eventlist
	std::vector<struct kevent>  	change_list;
    std::map<uintptr_t, RequestMessage>   fd_list;
public:
	SocketController();
	void socketInit();
	void kqueueInit();
	void  change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	void disconnect_client(int client_fd, std::map<int, std::string>& clients);

    void sendMessage(uintptr_t fd);
};

#endif
