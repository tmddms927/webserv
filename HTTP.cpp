#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

/*
** HTTP class default constructor.
*/
HTTP::HTTP() : socket_fd(-1) {
    requestMessage.current = PRE_READ_REQUEST_HEADER;
    requestMessage.content_length = -1;
}

/*
** HTTP class constructor. socket_fd값을 받아와서 저장.
*/
HTTP::HTTP(uintptr_t _socket_fd) : socket_fd(_socket_fd) {
    requestMessage.current = PRE_READ_REQUEST_HEADER;
}
