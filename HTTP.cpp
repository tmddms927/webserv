#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"


HTTP::HTTP() : server(servers()) {

}

/*
** HTTP class constructor. socket_fd값을 받아와서 저장.
*/
HTTP::HTTP(servers const & _server, uintptr_t _socket_fd) : server(_server), socket_fd(_socket_fd) {
    requestMessage.current = REQ_REQUEST_LINE;
}
