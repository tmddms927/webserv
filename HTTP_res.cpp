#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"


void HTTP::setResponseFileDirectory(std::string const & str) {
	responseMessage.file_directory = str;
}

/*
** set response line
*/
void HTTP::setResponseLine() {
	// 수정
	responseMessage.response_line = "HTTP/1.1 ";
	// responseMessage.response_line += this->status;
	responseMessage.response_line += "201";
	responseMessage.response_line += " ";
	responseMessage.response_line += "haha";
}

/*
** GET header 설정
*/
#include <sstream>
void HTTP::setGETHeader() {
	//todo 수정!
	responseMessage.header = "Server: ";
	responseMessage.header += "Webserv";
	responseMessage.header += "\r\n";

	responseMessage.header += "Date: ";
	responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	responseMessage.header += "\r\n";

	responseMessage.header += "Content-Type: ";
	responseMessage.header += "text/html";
	responseMessage.header += "\r\n";

	responseMessage.header += "Content-Length: ";
	std::stringstream s;
	s << responseMessage.body.length();
	responseMessage.header += s.str();
	responseMessage.header += "\r\n";
}

/*
** POST header 설정
*/
void HTTP::setPOSTHeader() {
	//todo 수정!
	responseMessage.header = "Server: ";
	responseMessage.header += "Webserv";
	responseMessage.header += "\r\n";

	responseMessage.header += "Date: ";
	responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	responseMessage.header += "\r\n";
}

void HTTP::setErrorResponse() {
	responseMessage.header = "Server: ";
	responseMessage.header += "Webserv";
	responseMessage.header += "\r\n";

	responseMessage.header += "Date: ";
	responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	responseMessage.header += "\r\n";

	//default error page 있으면
}

void HTTP::setResponseBody(std::string const & str) {
	responseMessage.body = str;
}
