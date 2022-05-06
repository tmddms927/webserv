#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

/*
** HTTP class default constructor.
*/
HTTP::HTTP() : server(servers()) {
    resetHTTP();
}

/*
** HTTP class constructor. socket_fd값을 받아와서 저장.
*/
HTTP::HTTP(servers const & _server, uintptr_t _socket_fd) : server(_server), socket_fd(_socket_fd) {
    resetHTTP();
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

int const & HTTP::getStatus() {
    return status;
}

void HTTP::setStatus(int const & s) {
    this->status = s;
}

uintptr_t const & HTTP::getResponseFd() {
    return response_fd;
}

void HTTP::setResponseFd(uintptr_t const & s) {
    this->response_fd = s;
}

void HTTP::resetHTTP() {
    requestMessage.buf = "";
    requestMessage.request_line = "";
    requestMessage.unparsed_uri = "";
    requestMessage.method = "";
    requestMessage.path = "";
    requestMessage.header_in.clear();
    requestMessage.body = "";
    requestMessage.chunk.content = "";
    requestMessage.chunk.length = -1;
    requestMessage.non_body = false;
    requestMessage.content_length = -1;
    requestMessage.chunked = false;
    requestMessage.current = CLIENT_READ_REQ_LINE;

    responseMessage.header = "";
    responseMessage.body = "";

    status = 0;
    protocol_minor_version = 0;
    response_fd = -1;
}
