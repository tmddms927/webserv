#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"


void HTTP::setResponseFileDirectory(std::string const & str) {
	responseMessage.file_directory = str;
}

std::string const & HTTP::getResponseFileDirectory() {
	return responseMessage.file_directory;
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

/*
** GET header 설정
*/
void HTTP::setGETHeader() {
	//todo 수정!
	// responseMessage.header = "Server: ";
	// responseMessage.header += "Webserv";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Date: ";
	// responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Content-Type: ";
	// responseMessage.header += "text/html";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Content-Length: ";
	// std::stringstream s;
	// s << responseMessage.body.length();
	// responseMessage.header += s.str();
	// responseMessage.header += "\r\n";
}

/*
** POST header 설정
*/
void HTTP::setPOSTHeader() {
	//todo 수정!
	// responseMessage.header = "Server: ";
	// responseMessage.header += "Webserv";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Date: ";
	// responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	// responseMessage.header += "\r\n";


	// //////////////
	// responseMessage.header += "Content-Type: ";
	// responseMessage.header += "text/html";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Content-Length: ";
	// std::stringstream s;
	// s << responseMessage.body.length();
	// responseMessage.header += s.str();
	// responseMessage.header += "\r\n";
}

/*
** PUT
*/
void HTTP::setPUTHeader() {
	//todo 수정!
	// responseMessage.header = "Server: ";
	// responseMessage.header += "Webserv";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Date: ";
	// responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Content-Type: ";
	// responseMessage.header += "text/html";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Content-Length: ";
	// std::stringstream s;
	// s << responseMessage.body.length();
	// responseMessage.header += s.str();
	// responseMessage.header += "\r\n";
}

/*
** DELETE
*/
void HTTP::setDELETEHeader() {
	// responseMessage.header = "Server: ";
	// responseMessage.header += "Webserv";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Date: ";
	// responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	// responseMessage.header += "\r\n";
}

/*
** HEAD
*/
void HTTP::setHEADHeader() {
	// //todo 수정!
	// responseMessage.header = "Server: ";
	// responseMessage.header += "Webserv";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Date: ";
	// responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	// responseMessage.header += "\r\n";
}

/*
** Error
*/
void HTTP::setErrorResponse() {
	// responseMessage.header = "Server: ";
	// responseMessage.header += "Webserv";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Date: ";
	// responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Content-Type: ";
	// responseMessage.header += "text/html";
	// responseMessage.header += "\r\n";

	// responseMessage.header += "Content-Length: ";
	// std::stringstream s;
	// s << responseMessage.body.length();
	// responseMessage.header += s.str();
	// responseMessage.header += "\r\n";
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
