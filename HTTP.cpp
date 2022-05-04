#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

/*
** HTTP class default constructor.
*/
HTTP::HTTP() : server(servers()) {
requestMessage.current = PRE_READ_REQUEST_HEADER;
    requestMessage.content_length = -1;
}

/*
** HTTP class constructor. socket_fd값을 받아와서 저장.
*/
HTTP::HTTP(servers const & _server, uintptr_t _socket_fd) : server(_server), socket_fd(_socket_fd) {
    requestMessage.current = PRE_READ_REQUEST_HEADER;
}
