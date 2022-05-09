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
<<<<<<< HEAD:Server.hpp
#include "HTTP/HTTP.hpp"
#include "config/Config.hpp"
#include "utils.hpp"

#define SOCKET_LISTEN_BACKLOG   5
#define SOCKET_PORT             80
#define SOCKET_ADDR             INADDR_ANY
#define SOCKET_READ_BUF         16384
#define KQUEUE_EVENT_LIST_SIZE  1024
#define REQUEST_BODY_MAX_SIZE   100000001
#define RECIEVE_BODY_MAX_SIZE   16384
#define SERVER_DEFAULT_NAME		"webserv"
=======
#include "../HTTP.hpp"
#include "../config/Config.hpp"
#include "../utils.hpp"
#include "Server_define.hpp"
>>>>>>> 7105c08d7c71e5e2881cff1c3f4bc42e87480915:Server/Server.hpp

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
	std::map<uintptr_t, size_t>		file_fd;

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
	void	kqueueInit();
	void	addServerSocketKevent();
	void	kqueueEventRun();
	void	checkKeventFilter();
	void	kqueueEventError();
	void	kqueueEventRead();
	void	kqueueConnectAccept();
	void	kqueueEventReadClient();
	void	kqueueEventReadFileFd();
	void	finishedRead();
	void	checkMethod();
	void	kqueueEventWrite();

	/* Server_method */
    void	findServerBlock();

	void	setResErrorMes(int const & client);
	void	setResMethodGET();
	void	setResMethodPOST();
	void	setResMethodPUT();
	void	setResMethodDELETE();
	void	setResMethodHEAD();

	void	readResErrorFile();
	void	readResGETFile();
	void	writeResPOSTFile();
	void	readResHEADFile();

	void	sendResMessage();
	void	setResDefaultHeaderField();
	void	changeStatusToError(int const & client, int const & st);
	void	isFile();

	/* utils.cpp */
	void	change_events(uintptr_t const & ident,
			int16_t const & filter, uint16_t const & flags);
	void	disconnect_client(uintptr_t fd);
	int		checkServerSocket(uintptr_t const & fd) const;
	bool	checkFileFd() const;
	void	disconnect_file_fd();
	void	checkClientTimeout();
	void	checkAllowedMethod();
};

#endif
