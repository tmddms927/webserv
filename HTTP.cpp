#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

/*
** HTTP class default constructor.
*/
HTTP::HTTP(){
    resetHTTP();
}

/*
** HTTP class constructor. socket_fd값을 받아와서 저장.
*/
HTTP::HTTP(uintptr_t _server_fd) : server_fd(_server_fd) {
    resetHTTP();
}

uintptr_t const & HTTP::getServerFd() const {
    return server_fd;
}

std::string const & HTTP::getMethod() const {
    return requestMessage.method;
}

std::string const & HTTP::getURI() const {
    return requestMessage.unparsed_uri;
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

// uintptr_t const & HTTP::getResponseFd() {
//     return response_fd;
// }

// void HTTP::setResponseFd(uintptr_t const & s) {
//     this->response_fd = s;
// }

void HTTP::resetHTTP() {
    requestMessage.buf = "";
    requestMessage.request_line = "";
    requestMessage.unparsed_uri = "";
    requestMessage.method = "";
    requestMessage.header_in.clear();
    requestMessage.body = "";
    requestMessage.chunk.initChunk();
    requestMessage.non_body = false;
    requestMessage.content_length = -1;
    requestMessage.chunked = false;
    requestMessage.request_step = CLIENT_READ_REQ_LINE;

    responseMessage.file_directory = "";
    responseMessage.step = -1;
    responseMessage.response_line = "";
    responseMessage.header = "";
    responseMessage.body = "";

    status = 0;
    // response_fd = -1;
}

std::string const & HTTP::getResponseLine() {
    return responseMessage.response_line;
}

std::string const & HTTP::getResponseHeader() {
    return responseMessage.header;
}

std::string const & HTTP::getResponseBody() {
    return responseMessage.body;
}
