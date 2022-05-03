#include "SocketController.hpp"

/*
** server main socket를 kqueue에 등록
*/
void SocketController::kqueueInit() {
    this->kq = kqueue();
	if (this->kq == -1)
		throw "kqueue() error!";
	change_events(server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "<<< server started! >>>" << std::endl;
}

/*
** check kqueue event
*/
void SocketController::kqueueEventRun() {
    int new_events;

    while (1) {
        new_events = kevent(kq, &change_list, 1, &event_list, KQUEUE_EVENT_LIST_SIZE, NULL);
        // std::cout << new_events << ", " << event_list.filter << ", " << event_list.ident << std::endl;
        if (new_events == -1)
            throw "kevent() error";
        if (event_list.flags == EV_ERROR) {
            // std::cout << "hi1" << std::endl;
            kqueueEventError();
            exit(1);
        }
        if (event_list.filter == EVFILT_READ) {
            // std::cout << "hi2" << std::endl;
            kqueueEventRead();
        }
        if (event_list.filter == EVFILT_WRITE) {
            // std::cout << "hi3" << std::endl;
            kqueueEventWrite();
        }
    }
}

/*
** read event의 fd가 server_socket인지 client socket인지 확인
*/
void SocketController::kqueueEventRead() {
    if (event_list.ident == server_socket)
        kqueueConnectAccept();
    else if (fd_list.find(event_list.ident) != fd_list.end())
        kqueueEventReadClient();
}

/*
** accept new client & set non-block file & add kqueue event
*/
void SocketController::kqueueConnectAccept(void) {
    int client_socket;

    if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
        throw "accept() error";
    std::cout << "accept new client : " << client_socket << std::endl;
    fcntl(client_socket, F_SETFL, O_NONBLOCK);
    change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    change_events(client_socket, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
    fd_list.insert(std::pair<uintptr_t, HTTP>(client_socket, HTTP(event_list.ident)));
}

/*
** read data from client
*/
void SocketController::kqueueEventReadClient() {
    char buf[SOCKET_READ_BUF];
    int n;

    while (1) {
        std::memset(buf, 0, SOCKET_READ_BUF);
        n = read(event_list.ident, buf, SOCKET_READ_BUF - 1);
        if (n == 0) {
            std::cerr << "client read error!" << std::endl;
            disconnect_client(event_list.ident);
        }
        else if (n == - 1) {
            // todo finishRead를 다 읽었는지 확인 후로 바꿔야돰!
            finishRead();
            break;
        }
        else
            fd_list[event_list.ident].reqInputBuf(buf);
    }
}

/*
** read를 다 했을 경우 write 이벤트를 등록해줌.
*/
void SocketController::finishRead() {
    change_events(event_list.ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
}

/*
** send data to client
*/
void SocketController::kqueueEventWrite() {
    if (fd_list[event_list.ident].reqCheckFinished() == true) {
        fd_list[event_list.ident].reqPrint();
        fd_list[event_list.ident].resSendMessage();
        finishWrite();
    }
}

/*
** write를 다 했을 경우 write 이벤트를 삭제해줌.
*/
void SocketController::finishWrite() {
    change_events(event_list.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
}

/*
** kqueue에서 error가 난 경우.
** server main socket : socket 다시 생성
** client listen socket : fd_list에서 fd 삭제
*/
void SocketController::kqueueEventError(){
    if (event_list.ident == server_socket) {
		//todo server socket 다시 init 해주기!
        throw "server socket error";
    }
    else
    {
        std::cerr << "client socket error" << std::endl;
        disconnect_client(event_list.ident);
    }
}

/*
** connect client : add kqueue event & add fd_list
*/
void SocketController::change_events(uintptr_t ident, int16_t filter, uint16_t flags,
                                    uint32_t fflags, intptr_t data, void *udata)
{
    EV_SET(&change_list, ident, filter, flags, fflags, data, udata);
    kevent(kq, &change_list, 1, &event_list, KQUEUE_EVENT_LIST_SIZE, NULL);
}

/*
** disconnect client : close fd & erase fd at fd_list
*/
void SocketController::disconnect_client(int client_fd)
{
	std::cout << "client disconnected : " << client_fd << std::endl;
	close(client_fd);
    fd_list.erase(client_fd);
}

//GET / HTTP/1.1**Host: 127.0.0.1**Content-Length: 1****a****

// ]
