#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"
#include "../utils.hpp"

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
    status = 0;
    req_finished_time = get_time();

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

    responseMessage.have_file_fd = false;
    responseMessage.file_directory = "";
    responseMessage.response_line = "";
    responseMessage.header = "";
    responseMessage.body = "";
}

std::string const & HTTP::getResponseLine() const {
    return responseMessage.response_line;
}

std::string const & HTTP::getResponseHeader() const {
    return responseMessage.header;
}

std::string const & HTTP::getResponseBody() const {
    return responseMessage.body;
}

bool const & HTTP::getResponseHaveFileFd() const {
    return responseMessage.have_file_fd;
}

void HTTP::setResponseHaveFileFd(bool const & have) {
    responseMessage.have_file_fd = have;
}

unsigned long const & HTTP::getReqFinishedTime() {
    return req_finished_time;
}