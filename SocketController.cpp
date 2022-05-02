#include "SocketController.hpp"

SocketController::SocketController() {
    timeout.tv_nsec = 0;
    timeout.tv_sec = 0;
}

void SocketController::socketRun() {
    try {
        socketInit();
        kqueueInit();
        kqueueEventRun();
    }
    catch(const char * err) {
        std::cout << err << std::endl;
    }
}


void SocketController::socketInit() {
	if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw "socket() error!";
    sock_opt = 0;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt));
	// sockaddr_in->sin_zero가 반드시 0으로 채워져 있어야 함.
	// https://techlog.gurucat.net/292
	memset(&server_addr, 0, sizeof(server_addr));
	// AF_INET = TCP 통신
	// sin_family는 커널에서 사용하므로 little endian를 저장.
	server_addr.sin_family = AF_INET;
	// sin_addr, sin_port는 네트워크 통신에 사용하므로 big endian으로 바꿔줘야 됨.
	// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=lovinghc&logNo=30031089847
	server_addr.sin_addr.s_addr = htonl(SOCKET_ADDR);
	server_addr.sin_port = htons(SOCKET_PORT);

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		throw "bind() error!";

	if (listen(server_socket, SOCKET_LISTEN_BACKLOG) == -1)
		throw "listen() error!";
	// server_socket를 nonblock으로 설정.
	fcntl(server_socket, F_SETFL, O_NONBLOCK);
	std::cout << "server socker init complete!" << std::endl;
}
