#include "Server.hpp"

void Server::methodGet() {
    int fd;

    fd = open(clients[curr_event->ident].getURI().c_str(), O_RDONLY);
    // todo open fail 처리
    subrequest_fd.insert(std::pair<uintptr_t, uintptr_t>(fd, curr_event->ident));
    // change_events(curr_event->ident, EVFILT_READ, EV_DISABLE);
    std::cout << "file fd resister" << std::endl;
}

void Server::methodPost() {
    int fd;

    fd = open(clients[curr_event->ident].getURI().c_str(), O_WRONLY);
    // todo open fail 처리
    subrequest_fd.insert(std::pair<uintptr_t, uintptr_t>(fd, curr_event->ident));
    // change_events(curr_event->ident, EVFILT_READ, EV_DISABLE);
    std::cout << "file fd resister" << std::endl;
}
