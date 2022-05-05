#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

/*
** HTTP class default constructor.
*/
HTTP::HTTP() : server(servers()) {
requestMessage.current = CLIENT_READ_REQ_LINE;
    requestMessage.content_length = -1;
}

/*
** HTTP class constructor. socket_fd값을 받아와서 저장.
*/
HTTP::HTTP(servers const & _server, uintptr_t _socket_fd) : server(_server), socket_fd(_socket_fd) {
    requestMessage.current = CLIENT_READ_REQ_LINE;
}


std::string const & HTTP::getMethod() const {
    return requestMessage.method;
}

std::string const & HTTP::getURI() const {
    return requestMessage.path;
}

std::string & HTTP::getBody() {
    return requestMessage.body;
}