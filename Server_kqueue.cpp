#include "Server.hpp"

/*
** server main socket를 kqueue에 등록
*/
void Server::kqueueInit() {
	this->kq = kqueue();
	if (this->kq == -1)
		throw "kqueue() error!";

	int size = config.size();
	for (int i = 0; i < size; ++i)
    	change_events(server_socket[i], EVFILT_READ, EV_ADD | EV_ENABLE);
	std::cout << "<<< server started! >>>" << std::endl;
}

/*
** check kqueue event
*/
void Server::kqueueEventRun() {
	int new_events;

	while (1) {
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 1024, NULL);
        std::cout << "kqueueEventRun : event occur" << std::endl;
        change_list.clear();

		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];
			
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
	if (checkServerSocket(curr_event->ident) != -1)
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
	if (checkServerSocket(curr_event->ident) != -1)
		kqueueConnectAccept();
	// else
	// 	kqueueEventReadClient();
	else if (checkClientSocket(curr_event->ident) != -1)
		kqueueEventReadClient(); //kqueueEventReadClientRequest()
	else if (checkFileDescriptor(curr_event->ident) != -1)
		GETMethod(curr_event->ident, clients[curr_event->ident].getBody());
}

/*
** accept new client & set non-block file & add kqueue event
*/
void Server::kqueueConnectAccept() {
	int client_socket;
	int server_index = checkServerSocket(curr_event->ident);

	client_socket = accept(server_socket[server_index], NULL, NULL);
	// todo accept 실패 시 어떻게 할 것인지
	if (client_socket == -1)
		throw "accept() error";
	std::cout << "accept new client : " << client_socket << std::endl;
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE);
	change_events(client_socket, EVFILT_WRITE, EV_ADD | EV_DISABLE);
	clients.insert(std::pair<uintptr_t, HTTP>(client_socket, HTTP(config[server_index], static_cast<uintptr_t>(client_socket))));
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
		if (n == 0) {
			std::cerr << "client read error!" << std::endl;
			disconnect_client();
		}
		else if (n < 0)
			break;
		else
			clients[curr_event->ident].reqInputBuf(buf);
	}
	if (clients[curr_event->ident].reqCheckFinished()) {
		// change_events(curr_event->ident, EVFILT_WRITE, EV_ENABLE);
		if (clients[curr_event->ident].getMethod() == GET) {
			int fd;
			
			fd = open(clients[curr_event->ident].getURI().c_str(), O_RDONLY);
			change_events(fd, EVFILT_READ, EV_ADD | EV_ENABLE);
			subrequest_fd.insert(std::pair<uintptr_t, uintptr_t>(fd, curr_event->ident));
			change_events(curr_event->ident, EVFILT_READ, EV_ADD | EV_DISABLE);
            std::cout << "file fd resister" << std::endl;
		}
	}
}

/*
** send data to client
*/
void Server::kqueueEventWrite() {
	clients[curr_event->ident].reqPrint();
	clients[curr_event->ident].resSendMessage();
	// todo write buf 크기가 나눠져 있으면
	change_events(curr_event->ident, EVFILT_WRITE, EV_DISABLE);
}

/*
** connect client : add kqueue event & add fd_list
*/
void Server::change_events(uintptr_t const & ident, int16_t const & filter, uint16_t const & flags)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, 0, 0, NULL);
    change_list.push_back(temp_event);
}

/*
** disconnect client : close fd & erase fd at fd_list
*/
void Server::disconnect_client()
{
    std::cout << "client disconnected: " << curr_event->ident << std::endl;
    close(curr_event->ident);
    clients.erase(curr_event->ident);
}

int Server::checkServerSocket(uintptr_t const & fd) {
	int size = config.size();

	for (int i = 0; i < size; ++i) {
		if (fd == server_socket[i])
			return i;
	}
	return -1;
}

int	Server::checkClientSocket(uintptr_t const & fd) {
	if (clients.find(fd) == clients.end())
		return -1;
	return 1;
}

int Server::checkFileDescriptor(uintptr_t const & fd) {
	if (subrequest_fd.find(fd) == subrequest_fd.end())
		return -1;
	return 1;
}