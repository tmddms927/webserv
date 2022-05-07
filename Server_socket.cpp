#include "Server.hpp"

/*
** server socket 구동하기
*/
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

/*
** server socket init
*/
void Server::socketInit() {
	int size;
	int fd;

	size = config.size();
	for (int i = 0; i < size; ++i)
	{
		fd = checkPort(i, config[i].port);
		if (fd != -1) {
			server_socket.push_back(server_socket[fd]);
			continue;
		}
		if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			throw "socket() error!";

		int sock_opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt));

		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(SOCKET_ADDR);
		server_addr.sin_port = htons(config[i].port);

		std::cout << "Open Port = "<< config[i].port << std::endl;

		if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
			throw "bind() error!";

		if (listen(fd, SOCKET_LISTEN_BACKLOG) == -1)
			throw "listen() error!";

		fcntl(fd, F_SETFL, O_NONBLOCK);
		server_socket.push_back(static_cast<uintptr_t>(fd));
	}
	std::cout << "server socker init complete!" << std::endl;
}

/*
** request가 어떤 port로 들어왔는지 찾아주기
*/
uintptr_t Server::checkPort(int const & i, int const & port) const {
	for (int j = 0; j < i; ++j) {
		if (config[j].port == port) {
			return j;
		}
	}
	return -1;
}
