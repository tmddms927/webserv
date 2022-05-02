#include "SocketController.hpp"

/*
** server main socket를 kqueued에 등록
*/
void SocketController::kqueueInit() {
	if ((kq = kqueue()) == -1)
		throw "kqueue() error!";
	change_events(change_list, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "echo server started" << std::endl;
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
    change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
	change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    fd_list.insert(std::pair<uintptr_t, HTTP>(client_socket, HTTP(curr_event->ident)));
}

/*
** read data from client
*/
void SocketController::kqueueEventReadClient() {
    char buf[SOCKET_READ_BUF];
    int n;

    while (1) {
        std::memset(buf, 0, SOCKET_READ_BUF);
        n = read(curr_event->ident, buf, SOCKET_READ_BUF - 1);
        if (n == 0) {
            std::cerr << "client read error!" << std::endl;
            disconnect_client(curr_event->ident);
        }
        else if (n == - 1)
            break;
        else
            fd_list[curr_event->ident].reqInputBuf(buf);
    }
}

/*
** kqueue에서 error가 난 경우.
** server main socket : socket 다시 생성
** client listen socket : fd_list에서 fd 삭제
*/
void SocketController::kqueueEventError(){
    if (curr_event->ident == server_socket) {
		//todo server socket 다시 init 해주기!
        throw "server socket error";
    }
    else
    {
        std::cerr << "client socket error" << std::endl;
        disconnect_client(curr_event->ident);
    }
}

/*
** read event의 fd가 server_socket인지 client socket인지 확인
*/
void SocketController::kqueueEventRead() {
    if (curr_event->ident == server_socket)
        kqueueConnectAccept();
    else if (fd_list.find(curr_event->ident) != fd_list.end())
        kqueueEventReadClient();
}

/*
** send data to client
*/
void SocketController::kqueueEventWrite() {
    std::map<uintptr_t , HTTP>::iterator it = fd_list.find(curr_event->ident);

    if (it != fd_list.end())
    {
        if (it->second.reqCheckFinished() == true) {
            it->second.reqPrint();
            it->second.resSendMessage();
            std::cout << "finished!" << std::endl;
        }
    }

}

/*
** check kqueue event
*/
void SocketController::kqueueEventRun() {
    int new_events;

    while (1) {
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, KQUEUE_EVENT_LIST_SIZE, NULL);
        if (new_events == -1)
            throw "kevent() error";

        change_list.clear();

        for (int i = 0; i < new_events; ++i)
        {
            curr_event = &event_list[i];

            if (curr_event->flags & EV_ERROR)
                kqueueEventError();
            else if (curr_event->filter == EVFILT_READ)
                kqueueEventRead();
            else if (curr_event->filter == EVFILT_WRITE)
                kqueueEventWrite();
        }
    }
}

/*
** connect client : add kqueue event & add fd_list
*/
void SocketController::change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
                                     uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

/*
** disconnect client : close fd & erase fd at fd_list
*/
void SocketController::disconnect_client(int client_fd)
{
	std::cout << "client disconnected: " << client_fd << std::endl;
	close(client_fd);
    fd_list.erase(client_fd);
}
