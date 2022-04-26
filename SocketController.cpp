#include "SocketController.hpp"

SocketController::SocketController() {
	try {
		socketInit();
		kqueueInit();
	}
	catch(const char * err) {
		std::cout << err << std::endl;
	}
}

void SocketController::socketInit() {
	if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		throw "socket() error!";

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

void SocketController::kqueueInit() {
	// 커널에 새로운 event queue를 만들고 fd를 return
	// return된 fd는 kevent()에서 이벤트를 등록, 모니터링하는데 사용. 
	if ((kq = kqueue()) == -1)
		throw "kqueue() error!";
	// kq로 전달된 kqueue에 새로 모니터링할 이벤트를 등록.
	// 아직 처리되지 않은(pending 상태인) 이벤트의 개수를 return.
	/* add event for server socket */
	change_events(change_list, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "echo server started" << std::endl;

	int new_events;
	struct kevent* curr_event;
	std::map<int, std::string> clients; // map for client socket:data

	while (1)
	{
		/*  apply changes and return new events(pending events) */
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
		if (new_events == -1)
			throw "kevent() error";

		change_list.clear(); // clear change_list for new changes

		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];

			/* check error event return */
			if (curr_event->flags & EV_ERROR)
			{
				if (curr_event->ident == server_socket) {
					// server socket 다시 init 해주기!
					throw "server socket error";
				}
				else
				{
					std::cerr << "client socket error" << std::endl;
					disconnect_client(curr_event->ident, clients);
				}
			}
			else if (curr_event->filter == EVFILT_READ)
			{
				if (curr_event->ident == server_socket)
				{
					/* accept new client */
					int client_socket;
					if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
						throw "accept() error";
					std::cout << "accept new client : " << client_socket << std::endl;
					fcntl(client_socket, F_SETFL, O_NONBLOCK);

					/* add event for client socket - add read && write event */
					change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					clients[client_socket] = "";
				}
				else if (clients.find(curr_event->ident) != clients.end())
				{
					/* read data from client */
					char buf[1024];
                    RequestMessage rm;
                    if (fd_list.find(curr_event->ident) == fd_list.end())
                        fd_list.insert(std::pair<int, RequestMessage>(curr_event->ident, RequestMessage()));

					rm = fd_list[curr_event->ident];
                    int n = read(curr_event->ident, buf, sizeof(buf));
                    rm.inputBuf(buf);
                    std::cout << rm.getterBuf() << std::endl;
					if (n <= 0)
					{
						if (n < 0)
							std::cerr << "client read error!" << std::endl;
						disconnect_client(curr_event->ident, clients);
					}
					else
					{
						buf[n] = '\0';
						clients[curr_event->ident] += buf;
						std::cout << "received data from " << curr_event->ident << ": " << clients[curr_event->ident] << std::endl;
					}
				}
			}
			else if (curr_event->filter == EVFILT_WRITE)
			{
				/* send data to client */
				std::map<int, std::string>::iterator it = clients.find(curr_event->ident);
				if (it != clients.end())
				{
					if (clients[curr_event->ident] != "")
					{
						int n;
						if ((n = write(curr_event->ident, clients[curr_event->ident].c_str(),
										clients[curr_event->ident].size()) == -1))
						{
							std::cerr << "client write error!" << std::endl;
							disconnect_client(curr_event->ident, clients);  
						}
						else
							clients[curr_event->ident].clear();
					}
				}
			}
		}
	}
}

void SocketController::change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

void SocketController::disconnect_client(int client_fd, std::map<int, std::string>& clients)
{
	std::cout << "client disconnected: " << client_fd << std::endl;
	close(client_fd);
	clients.erase(client_fd);
}
