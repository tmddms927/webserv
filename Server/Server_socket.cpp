#include "Server.hpp"

/*
** server socket 구동하기
*/
void Server::socketRun() {
	try {
		ServerSocketInit();
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
void Server::ServerSocketInit() {
	int size;

	size = config.size();
	for (int i = 0; i < size; ++i)
		socketInit(i);
	std::cout << "server socker init complete!" << std::endl;
}

/*
** create socket
*/
void Server::socketInit(int const & i) {
	int fd;

	fd = checkPort(i, config[i].port);
	if (fd != -1)
	{
		server_socket.push_back(server_socket[fd]);
		return ;
	}
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw "socket() error!";

	removeBindError(fd);
	setSockaddr_in(i);

	if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		throw "bind() error!";

	if (listen(fd, SOCKET_LISTEN_BACKLOG) == -1)
		throw "listen() error!";

	fcntl(fd, F_SETFL, O_NONBLOCK);
	server_socket.push_back(static_cast<uintptr_t>(fd));
	std::cout << "Open Port = " << config[i].port << "(" << fd << ")" << std::endl;
}

/*
** 바인드 에러를 없애는 설정
*/
void Server::removeBindError(int const & fd) {
	int sock_opt = 1;

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt));
}

/*
** tcp 통신을 할 수 있도록 sockaddr_in 설정하기
*/
void Server::setSockaddr_in(int const & i) {
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(SOCKET_ADDR);
	server_addr.sin_port = htons(config[i].port);

}

/*
** request가 어떤 port로 들어왔는지 찾아주기
** i : index, port : port
*/
uintptr_t Server::checkPort(int const & i, int const & port) const {
	for (int j = 0; j < i; ++j) {
		if (config[j].port == port) {
			return j;
		}
	}
	return -1;
}
