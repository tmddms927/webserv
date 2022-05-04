#include "Server.hpp"

/*
** server main socket를 kqueue에 등록
*/
void Server::kqueueInit() {
	this->kq = kqueue();
	if (this->kq == -1)
		throw "kqueue() error!";
    change_events(server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "<<< server started! >>>" << std::endl;
}

/*
** check kqueue event
*/
void Server::kqueueEventRun() {
	int new_events;

	while (1) {
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 1024, NULL);
        change_list.clear();

		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];

			std::cout << new_events << ", " << curr_event->filter << ", " << curr_event->ident << std::endl;
			if (new_events == -1)
				throw "kevent() error";
			if (curr_event->flags & EV_ERROR)
				kqueueEventError();
			if (curr_event->filter == EVFILT_READ)
				kqueueEventRead();
			if (curr_event->filter == EVFILT_WRITE)
				kqueueEventWrite();
		}
	}
}

/*
** kqueue에서 error가 난 경우.
*/
void Server::kqueueEventError(){
	if (curr_event->ident == server_socket)
		throw "server socket error";
	else
	{
		std::cerr << "client socket error" << std::endl;
		disconnect_client();
	}
}

/*
** read event의 fd가 server_socket인지 client socket인지 확인
*/
void Server::kqueueEventRead() {
	if (curr_event->ident == server_socket)
		kqueueConnectAccept();
	else
		kqueueEventReadClient();
}

/*
** accept new client & set non-block file & add kqueue event
*/
void Server::kqueueConnectAccept(void) {
	int client_socket;

	client_socket = accept(server_socket, NULL, NULL);
	// todo accept 실패 시 어떻게 할 것인지
	if (client_socket == -1)
		throw "accept() error";
	std::cout << "accept new client : " << client_socket << std::endl;
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	change_events(client_socket, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
	clients_map.insert(std::pair<uintptr_t, HTTP>(client_socket, HTTP(client_socket)));
}

/*
** read data from client
*/
void Server::kqueueEventReadClient() {
	char buf[SOCKET_READ_BUF];
	int n;

	while (1) {
		std::memset(buf, 0, SOCKET_READ_BUF);
		n = read(curr_event->ident, buf, SOCKET_READ_BUF - 1);
		std::cout << n << std::endl;
		if (n == 0) {
			std::cerr << "client read error!" << std::endl;
			disconnect_client();
		}
		else if (n < 0)
			break;
		else
			clients_map[curr_event->ident].reqInputBuf(buf);
	}
	if (clients_map[curr_event->ident].reqCheckFinished()) {
		std::cout << "write change!!!!!!!" << std::endl;
		change_events(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
	}
}

/*
** send data to client
*/
void Server::kqueueEventWrite() {
	clients_map[curr_event->ident].reqPrint();
	clients_map[curr_event->ident].resSendMessage();
	// todo write buf 크기가 나눠져 있으면
	change_events(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
}

/*
** connect client : add kqueue event & add fd_list
*/
void Server::change_events(uintptr_t const & ident, int16_t const & filter, uint16_t const & flags,
					uint32_t const & fflags, intptr_t const & data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

/*
** disconnect client : close fd & erase fd at fd_list
*/
void Server::disconnect_client()
{
    std::cout << "client disconnected: " << curr_event->ident << std::endl;
    close(curr_event->ident);
    clients_map.erase(curr_event->ident);
}
