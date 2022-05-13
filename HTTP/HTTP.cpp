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

char HTTP::getMethod() const {
	return requestMessage.method;
}

std::string const & HTTP::getURI() const {
	return requestMessage.unparsed_uri;
}

void HTTP::setURI(std::string const & str) {
	requestMessage.unparsed_uri = str;
}

std::string & HTTP::getBody() {
	return requestMessage.body;
}

int const & HTTP::getStatus() {
	return status;
}

void HTTP::setStatus(int const & s) {
	if (this->status == 0 || this->status == -1)
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
	time_out = get_time();

	requestMessage.buf = "";
	requestMessage.request_line = "";
	requestMessage.unparsed_uri = "";
	requestMessage.method_name = "";
	requestMessage.method = 0;
	requestMessage.header_in.clear();
	requestMessage.body = "";
	requestMessage.chunk.initChunk();
	requestMessage.non_body = false;
	requestMessage.content_length = -1;
	requestMessage.chunked = false;
	requestMessage.request_step = CLIENT_READ_REQ_LINE;
	requestMessage.keep_alive = true;
	requestMessage.port_num = 0;

	responseMessage.server_block_index = 0;
	responseMessage.location_index = 0;
	responseMessage.cgi_index = -1;
	responseMessage.have_cgi_fd = false;
	responseMessage.have_file_fd = false;
	responseMessage.cgi_directory = "";
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

unsigned long long const & HTTP::getTimeOut() {
	return time_out;
}

void HTTP::setTimeOut() {
	time_out = get_time();
}

bool const & HTTP::getKeepAlive() const {
	return requestMessage.keep_alive;
}
