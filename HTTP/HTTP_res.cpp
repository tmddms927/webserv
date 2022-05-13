#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"


void HTTP::setResponseFileDirectory(std::string const & str) {
	responseMessage.file_directory = str;
}

std::string const & HTTP::getResponseFileDirectory() {
	return responseMessage.file_directory;
}

void HTTP::setResponseCGIDirectory(std::string const & str) {
	responseMessage.cgi_directory = str;
}

std::string const & HTTP::getResponseCGIDirectory() {
	return responseMessage.cgi_directory;
}

bool const & HTTP::getResponseHaveFileFd() const {
	return responseMessage.have_file_fd;
}

void HTTP::setResponseHaveFileFd(bool const & have) {
	responseMessage.have_file_fd = have;
}

bool const & HTTP::getResponseHaveCGIFd() const {
	return responseMessage.have_cgi_fd;
}

void HTTP::setResponseHaveCGIFd(bool const & have) {
	responseMessage.have_cgi_fd = have;
}

/*
** set response line
*/
#include <sstream>
void HTTP::setResponseLine() {
	// 수정

	responseMessage.response_line = "HTTP/1.1 ";

	std::stringstream stream;
	stream << this->status;
	responseMessage.response_line += stream.str();
	responseMessage.response_line += " ";
	responseMessage.response_line += "haha hoho";
}

void HTTP::setResponseBody(std::string const & str) {
	responseMessage.body = str;
}

bool HTTP::checkStatusError() {
	if (status >= 400)
		return true;
	else
		return false;
}

void HTTP::setResponseHeader(std::string const & key, std::string const & value) {
	responseMessage.header += key;
	responseMessage.header += ": ";
	responseMessage.header += value;
	responseMessage.header += "\r\n";
}
