#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"
#include "../utils/utils.hpp"

/*
** HTTP class default constructor.
*/
HTTP::HTTP(): cgi(), cgi_header_buf(), cgi_buf() {
	// std::cout << "HTTP default constructor called" << std::endl;
	resetHTTP();
}

void HTTP::setServerFd(uintptr_t const fd) {
	server_fd = fd;
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

void HTTP::setRedirectStatus(int const & s) {
	this->status = s;
}

void HTTP::setStatus(int const & s) {
	if (this->status == 0 || this->status == -1)
		this->status = s;
}

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
	requestMessage.host_name = "";
    cgi.CGI_clear();
	cgi_header_buf = "";
	cgi_buf = "";

	responseMessage.server_block_index = 0;
	responseMessage.location_index = 0;
	responseMessage.cgi_index = -1;
	responseMessage.have_cgi_fd = false;
	responseMessage.cgi_read_fd = -1;
	responseMessage.have_file_fd = false;
	responseMessage.cgi_directory = "";
	responseMessage.file_directory = "";
	responseMessage.response_line = "";
	responseMessage.header = "";
	responseMessage.body = "";
	responseMessage.res_step = CLIENT_RES_LINE;
	responseMessage.index = 0;
}

HTTP::~HTTP() {
	// std::cout << "HTTP destructor called" << std::endl;
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

std::string const	&	HTTP::getHostName() const {
	return requestMessage.host_name;
}

void HTTP::setTimeOut() {
	time_out = get_time();
}

bool const & HTTP::getKeepAlive() const {
	return requestMessage.keep_alive;
}

HTTP::HTTP(HTTP const &): cgi(), cgi_header_buf(), cgi_buf() {
	resetHTTP();
}

HTTP & HTTP::operator=(HTTP const &) {
	resetHTTP();
	return *this;
}