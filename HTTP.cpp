#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

/*
** HTTP class default constructor.
*/
HTTP::HTTP() : socket_fd(-1) {
    requestMessage.current = REQ_REQUEST_LINE;
}

/*
** HTTP class constructor. socket_fd값을 받아와서 저장.
*/
HTTP::HTTP(uintptr_t _socket_fd) : socket_fd(_socket_fd) {
    requestMessage.current = REQ_REQUEST_LINE;
}
