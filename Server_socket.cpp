#include "Server.hpp"

Server::Server() {
	this->kq_timeout.tv_nsec = 0;
	this->kq_timeout.tv_sec = 0;
}

void Server::socketRun() {
	try {
		socketInit();
		kqueueInit();
		kqueueEventRun();
	}
	catch(const char * err) {
		std::cout << err << std::endl;
	}
}

void Server::socketInit() {

	if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw "socket() error!";

	int sock_opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt));

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(SOCKET_ADDR);
	server_addr.sin_port = htons(SOCKET_PORT);

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		throw "bind() error!";

	if (listen(server_socket, SOCKET_LISTEN_BACKLOG) == -1)
		throw "listen() error!";

	fcntl(server_socket, F_SETFL, O_NONBLOCK);
	std::cout << "server socker init complete!" << std::endl;
}
