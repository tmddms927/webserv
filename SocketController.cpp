#include "SocketController.hpp"

void SocketController::kqueueConnectAccept(void) {
    /* accept new client */
    int client_socket;
    if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
        throw "accept() error";
    std::cout << "accept new client : " << client_socket << std::endl;
    fcntl(client_socket, F_SETFL, O_NONBLOCK);

    /* add event for client socket - add read && write event */
    change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
    //change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, NULL);
//    change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    //if (fd_list.find(curr_event->ident) == fd_list.end())
    fd_list.insert(std::pair<uintptr_t, HTTP>(client_socket, HTTP(curr_event->ident)));
    //todo 있을경우 생각!!!! 꼮!!!!!
}

void SocketController::kqueueEventReadClient() {
    /* read data from client */
    char buf[SOCKET_READ_BUF];
    int n;

    while (1) {
        std::memset(buf, 0, SOCKET_READ_BUF);
        n = read(curr_event->ident, buf, SOCKET_READ_BUF - 1);
        std::cout << "read num!!!!!!! " << n << std::endl;
        if (n == 0) {
            std::cerr << "client read error!" << std::endl;
            disconnect_client(curr_event->ident);
        }
        else if (n == - 1) {
            //fd_list[curr_event->ident].reqInputBuf(buf);
            break;
        }
        else {
            fd_list[curr_event->ident].reqInputBuf(buf);
//       change_events(change_list, curr_event->ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
//       kevent(kq, &change_list[0], change_list.size(), event_list, KQUEUE_EVENT_LIST_SIZE, &timeout);
//       change_events(change_list, curr_event->ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
            //std::cout << "READ END============================================" << std::endl;
            //std::cout << "ident " << curr_event->ident << std::endl;
        }
    }
}

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

void SocketController::kqueueEventRead() {
    //std::cout << "READ!!!!!!!!!!!!!!!!!" << std::endl;
    if (curr_event->ident == server_socket) {
        //std::cout << "read1!!!!!!!!" << std::endl;
        kqueueConnectAccept();
    }
    else if (fd_list.find(curr_event->ident) != fd_list.end()) {
        //std::cout << "read2!!!!!" << std::endl;
        kqueueEventReadClient();
    }
}

void SocketController::kqueueEventWrite() {
//    std::cout << "WRITE!!!!!!!!!!!!!!!!!" << std::endl;
    /* send data to client */
    std::map<uintptr_t , HTTP>::iterator it = fd_list.find(curr_event->ident);

    if (it != fd_list.end())
    {
        if (it->second.reqCheckFinished() == true) {
            it->second.reqPrint();
            it->second.resSendMessage();
            std::cout << "finished!" << std::endl;
        }
        /*
        if (fd_list[curr_event->ident]. != "")
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
         */
    }

}

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

}


void SocketController::disconnect_client(int client_fd)
{
	std::cout << "client disconnected: " << client_fd << std::endl;
	close(client_fd);
    fd_list.erase(client_fd);
}

void SocketController::sendMessage(uintptr_t fd) {
    std::string header = "HTTP/1.1 200 OK\r\n"
             "Server: nginx/1.21.6\r\n"
             "Date: Tue, 26 Apr 2022 10:59:45 GMT\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: 615\r\n"
             "Last-Modified: Tue, 25 Jan 2022 15:03:52 GMT\r\n"
             "Connection: keep-alive\r\n"
             "ETag: \"61f01158-267\"\r\n"
             "Accept-Ranges: bytes\r\n";
    std::string body = "<!DOCTYPE html>\r\n"
           "<html>\r\n"
           "<head>\r\n"
           "<title>Welcome to nginx!</title>\r\n"
           "<style>\r\n"
           "html { color-scheme: light dark; }\r\n"
           "body { width: 35em; margin: 0 auto;\r\n"
           "font-family: Tahoma, Verdana, Arial, sans-serif; }\r\n"
           "</style>\r\n"
           "</head>\r\n"
           "<body>\r\n"
           "<h1>Welcome to nginx!</h1>\r\n"
           "<p>If you see this page, the nginx web server is successfully installed and\r\n"
           "working. Further configuration is required.</p>\r\n"
           "\r\n"
           "<p>For online documentation and support please refer to\r\n"
           "<a href=\"http://nginx.org/\">nginx.org</a>.<br/>\r\n"
           "Commercial support is available at\r\n"
           "<a href=\"http://nginx.com/\">nginx.com</a>.</p>\r\n"
           "\r\n"
           "<p><em>Thank you for using nginx.</em></p>\r\n"
           "</body>\r\n"
           "</html>\r\n";
    write(fd, header.c_str(),header.size());
    write(fd, "\\r\\n", 2);
    write(fd, header.c_str(), header.size());
    write(fd, "\\r\\n", 2);
}

void SocketController::kqueueEventRun() {
    int new_events;
    std::map<int, std::string> clients; // map for client socket:data
    //todo client 삭제~
    while (1)
    {
        /*  apply changes and return new events(pending events) */
        std::cout << "kqueueEventRun start" << std::endl;
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, KQUEUE_EVENT_LIST_SIZE, NULL);
        //std::cout << "new event : " << new_events << std::endl;
        if (new_events == -1)
            throw "kevent() error";

        change_list.clear(); // clear change_list for new changes
        //std::cout << "change_list size : " << change_list.size() << std::endl;

        for (int i = 0; i < new_events; ++i)
        {
            //std::cout << "[" << i << "]" << std::endl;
            curr_event = &event_list[i];

            /* check error event return */
            if (curr_event->flags & EV_ERROR)
                kqueueEventError();
            else if (curr_event->filter == EVFILT_READ)
                kqueueEventRead();
            else if (curr_event->filter == EVFILT_WRITE)
                kqueueEventWrite();
            //std::cout << "[" << i << "]" << std::endl;
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
