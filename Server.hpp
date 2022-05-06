#ifndef Server_HPP
#define Server_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/event.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include "HTTP.hpp"
#include "config/Config.hpp"

#define SOCKET_LISTEN_BACKLOG   5
#define SOCKET_PORT             80
#define SOCKET_ADDR             INADDR_ANY
#define SOCKET_READ_BUF         16384
#define KQUEUE_EVENT_LIST_SIZE  1024
#define RECIEVE_BODY_MAX_SIZE	16384

/* SOCKET TYPE */
#define SOCKET_READ 0
#define FILE_READ 1

/*
** socket를 관리해주는 객체
*/
class Server {
private:
	// socket
	const std::vector<servers>		config;
	const global					global_config;
	std::vector<uintptr_t>			server_socket;
	sockaddr_in						server_addr;
	// kqueue
	int								kq;
	struct kevent					event_list[KQUEUE_EVENT_LIST_SIZE];
	struct kevent*					curr_event;
	std::map<uintptr_t, HTTP>		clients;
	struct timespec					kq_timeout;
	std::vector<struct kevent>		change_list;
	
	//GET, POST, LOGGING, CGI?등 하위 요청을 처리하기 위한 file descriptior
	//<file_fd, socket_fd>
	// std::map<uintptr_t, uintptr_t>	subrequest_fd;

public:
	Server(Config const & c);
	/* Server_socket */
	void socketInit();
	void socketRun();
	uintptr_t checkPort(int const & i, int const & port) const;

	/* Server_kqueue */
	void kqueueInit();
	void kqueueEventRun();

	/* in kqueue eventRun */
	void kqueueEventError();
	void kqueueEventRead();
	void kqueueConnectAccept();
	void kqueueEventReadClient();
	void finishedRead();
	void kqueueEventWrite();
	void disconnect_client();

	int checkServerSocket(uintptr_t const & fd);
	// int	checkClientSocket(uintptr_t const & fd);
	// int checkFileDescriptor(uintptr_t const & fd);

	void change_events(uintptr_t const & ident, int16_t const & filter, uint16_t const & flags);

	/* Server_method */
    void findServerBlock();
	void setError();
	void setMethodGet();
	void setMethodPost();
	void resSendMessage();
};

#endif
