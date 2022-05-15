#include "Server.hpp"
#include "URIParser/URIParser.hpp"

/*
** server main socket를 kqueue에 등록
*/
void Server::kqueueInit() {
	this->kq = kqueue();
	if (this->kq == -1) {
		closeAllFd();
		throw "kqueue() error!";
	}
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
							event_list, KQUEUE_EVENT_LIST_SIZE, &kq_timeout);
		change_list.clear();

		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];
			if (new_events == -1) {
				closeAllFd();
				throw "kevent() error";
			}
			try {
				checkKeventFilter();
			}
			catch(const char * err) {
				throw err;
			}
		}
		// checkClientTimeout();
	}
}

/*
** check kqueue filter
*/
void Server::checkKeventFilter() {
	try {
		if (curr_event->flags & EV_ERROR)
			kqueueEventError();
		if (curr_event->filter == EVFILT_READ)
			kqueueEventRead();
		if (curr_event->filter == EVFILT_WRITE)
			kqueueEventWrite();
	}
	catch(const char * err) {
		throw err;
	}
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
	try {
		if (checkServerSocket(curr_event->ident) != -1)
			kqueueConnectAccept();
		else if (checkFileFd())
			kqueueEventReadFileFd();
		else if (checkCGIFd())
			readCGI();
		else
			kqueueEventReadClient();
	}
	catch(const char * err) {
		throw err;
	}
}

/*
** accept new client & set non-block file & add kqueue event
*/
void Server::kqueueConnectAccept() {
	int client_socket;
	int server_index = checkServerSocket(curr_event->ident);

	client_socket = accept(server_socket[server_index], NULL, NULL);
	if (client_socket == -1)
		throw "accept() error";
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE);
	change_events(client_socket, EVFILT_WRITE, EV_ADD | EV_DISABLE);
	clients.insert(std::pair<uintptr_t, HTTP>(client_socket, HTTP()));
	clients[client_socket].setServerFd(curr_event->ident);
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
	clients[curr_event->ident].setTimeOut();
	if (n == 0) {
		std::cerr << "client read error!" << std::endl;
		disconnect_client(curr_event->ident);
	}
	else
		clients[curr_event->ident].reqInputBuf(buf);
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
	URIParser uriParser(clients[curr_event->ident], server_socket, config);
	uriParser.checkReqHeader();
	checkMaxBodySize();
	
	if (checkRedirect()) {
		change_events(curr_event->ident, EVFILT_WRITE, EV_ENABLE);
		return ;
	}
	if (clients[curr_event->ident].getResponseCGIDirectory() != "")
		setClientCGI();
	else
		checkMethod();
	if (clients[curr_event->ident].getResponseHaveFileFd() == false && 
		clients[curr_event->ident].getResponseHaveCGIFd() == false) {
		change_events(curr_event->ident, EVFILT_WRITE, EV_ENABLE);
	}
}

/*
** check GGI
*/
void Server::setClientCGI() {
	uintptr_t write_fd;
	uintptr_t read_fd;
	pid_t pid;

	clients[curr_event->ident].cgi_creat(write_fd, read_fd, pid);
	change_events(write_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
	change_events(read_fd, EVFILT_READ, EV_ADD | EV_ENABLE);
	cgi_fd[write_fd] = curr_event->ident;
	cgi_fd[read_fd] = curr_event->ident;
	clients[curr_event->ident].setResponseHaveCGIFd(true);
}

/*
** check method
*/
void Server::checkMethod() {
	if (clients[curr_event->ident].getStatus() == 200)
		;
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
		if (clients[file_fd[curr_event->ident]].getMethod() == PUT_BIT)
			writeResPUTFile();
		disconnect_file_fd();
	}
	else if (checkCGIFd())
		writeCGI();
	else
		sendResMessage();
}
