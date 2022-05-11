#include "Server.hpp"

/*
** server main socket를 kqueue에 등록
*/
void Server::kqueueInit() {
	this->kq = kqueue();
	if (this->kq == -1)
		throw "kqueue() error!";
	addServerSocketKevent();
}

/*
** server socket를 kqueue에 이벤트 등록
*/
void Server::addServerSocketKevent() {
	int size = server_socket.size();

	for (int i = 0; i < size; ++i)
		change_events(server_socket[i], EVFILT_READ, EV_ADD | EV_ENABLE);
	std::cout << "<<< server started!!! >>>" << std::endl;
}

/*
** check kqueue event
*/
void Server::kqueueEventRun() {
	int new_events;

	while (1) {
		new_events = kevent(kq, &change_list[0], change_list.size(),
							event_list, KQUEUE_EVENT_LIST_SIZE, NULL);
		change_list.clear();

		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];
			std::cout << curr_event->ident << ", " << curr_event->filter << std::endl;
			if (new_events == -1)
				throw "kevent() error";
			checkKeventFilter();
		}
		checkClientTimeout();
	}
}

/*
** check kqueue filter
*/
void Server::checkKeventFilter() {
	if (curr_event->flags & EV_ERROR) {
		kqueueEventError();
		exit(1);
	}
	if (curr_event->flags & EV_EOF)
		kqueueEventError();
	if (curr_event->filter == EVFILT_READ)
		kqueueEventRead();
	if (curr_event->filter == EVFILT_WRITE)
		kqueueEventWrite();
}

/*
** kqueue에서 error가 난 경우.
*/
void Server::kqueueEventError() {
	if (checkServerSocket(curr_event->ident) != -1)
		throw "server socket error";
	else
	{
		std::cerr << "client socket error" << std::endl;
		disconnect_client(curr_event->ident);
	}
}

/*
** read event의 fd가 server_socket인지 client socket인지 확인
*/
void Server::kqueueEventRead() {
	if (checkServerSocket(curr_event->ident) != -1) {
		kqueueConnectAccept();
	}
	else if (checkFileFd())
		kqueueEventReadFileFd();
	else
		kqueueEventReadClient();
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
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE);
	change_events(client_socket, EVFILT_WRITE, EV_ADD | EV_DISABLE);
	clients.insert(std::pair<uintptr_t, HTTP>(client_socket, HTTP(curr_event->ident)));
	std::cout << "accept new client : " << client_socket << std::endl;
}

/*
** read data from client
*/
void Server::kqueueEventReadClient() {
	char buf[SOCKET_READ_BUF];
	int n;

	std::memset(buf, 0, SOCKET_READ_BUF);
	n = read(curr_event->ident, buf, SOCKET_READ_BUF - 1);
	std::cout << "[" << buf << "]" << std::endl;
	if (n == 0) {
		std::cerr << "client read error!" << std::endl;
		disconnect_client(curr_event->ident);
	}
	else
		clients[curr_event->ident].reqInputBuf(buf);
	if (clients[curr_event->ident].reqCheckHeaderFinished())
		checkReqHeader();
	if (clients[curr_event->ident].reqCheckFinished())
		finishedRead();
}

/*
** read data from fd
*/
void Server::kqueueEventReadFileFd() {
	int fd;

	fd = file_fd[curr_event->ident];
	if (clients[fd].getStatus() != 0 && clients[fd].getStatus() != -1)
		readResErrorFile();
	else if (clients[fd].getMethod() == GET_BIT)
		readResGETFile();
	else if (clients[fd].getMethod() == HEAD_BIT)
		readResHEADFile();
	disconnect_file_fd();
}

/*
** read가 끝났을 때 처리
*/
void Server::finishedRead() {
	change_events(curr_event->ident, EVFILT_READ, EV_DISABLE);

	checkAllowedMethod();
	checkReqHeader();
	checkMethod();
	if (clients[curr_event->ident].getResponseHaveFileFd() == false)
		change_events(curr_event->ident, EVFILT_WRITE, EV_ENABLE);
}

/*
** check method
*/
void Server::checkMethod() {
	if (clients[curr_event->ident].getStatus() == 200)
		setResOKMes();
	else if (clients[curr_event->ident].getStatus() != 0 && clients[curr_event->ident].getStatus() != -1)
		setResErrorMes(curr_event->ident);
	else if (clients[curr_event->ident].getMethod() == GET_BIT)
		setResMethodGET();
	else if (clients[curr_event->ident].getMethod() == POST_BIT)
		setResMethodPOST();
	else if (clients[curr_event->ident].getMethod() == DELETE_BIT)
		setResMethodDELETE();
	else if (clients[curr_event->ident].getMethod() == HEAD_BIT)
		setResMethodHEAD();
	else if (clients[curr_event->ident].getMethod() == PUT_BIT)
		setResMethodPUT();
}

/*
** send data to client
*/
void Server::kqueueEventWrite() {
	if (checkFileFd()) {
		writeResPOSTFile();
		disconnect_file_fd();
	}
	else {
		sendResMessage();
		change_events(curr_event->ident, EVFILT_WRITE, EV_DISABLE);
		change_events(curr_event->ident, EVFILT_READ, EV_ENABLE);
		checkKeepAlive();
		clients[curr_event->ident].resetHTTP();
	}
}
