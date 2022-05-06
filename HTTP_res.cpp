#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

/*
** set response line
*/
void HTTP::setResponseLine() {
	responseMessage.step = CLIENT_RES_LINE;
	responseMessage.response_line += "HTTP/1.1 ";
	responseMessage.response_line += this->status;
	responseMessage.response_line += " ";
	responseMessage.response_line += "haha";
}

/*
** GET header 설정
*/
void HTTP::setGETHeader() {
	//todo 수정!
	responseMessage.header += "Server: ";
	responseMessage.header += "Webserv";
	responseMessage.header += "\r\n";

	responseMessage.header += "Date: ";
	responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	responseMessage.header += "\r\n";

	responseMessage.header += "Content-Type: ";
	responseMessage.header += "text/html";
	responseMessage.header += "\r\n";

	responseMessage.header += "Transfer-Encoding: ";
	responseMessage.header += "chunked\r\n";
}

/*
** POST header 설정
*/
void HTTP::setPOSTHeader() {
	//todo 수정!
	responseMessage.header += "Server: ";
	responseMessage.header += "Webserv";
	responseMessage.header += "\r\n";

	responseMessage.header += "Date: ";
	responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	responseMessage.header += "\r\n";
}

void HTTP::setErrorResponse() {
	responseMessage.header += "Server: ";
	responseMessage.header += "Webserv";
	responseMessage.header += "\r\n";

	responseMessage.header += "Date: ";
	responseMessage.header += "Tue, 26 Apr 2022 10:59:45 GMT";
	responseMessage.header += "\r\n";

	//default error page 있으면
}

void HTTP::resSendMessage() {
	size_t remain = 16384;

	if (responseMessage.step == CLIENT_RES_LINE) {
		if (resSendSize(remain, responseMessage.response_line))
			responseMessage.step = CLIENT_RES_HEADER;
	}
	if (responseMessage.step == CLIENT_RES_HEADER && remain > 0) {
		if (resSendSize(remain, responseMessage.header))
			responseMessage.step = CLIENT_RES_BODY;
	}
	if (responseMessage.step == CLIENT_RES_BODY && remain > 0) {
		if (response_fd != -1) {
			std::cout << "wowo!" << std::endl;
			if (resSendFD(remain))
				responseMessage.step = CLIENT_RES_FINISH;
		}
		else {
			if (resSendSize(remain, responseMessage.body))
				responseMessage.step = CLIENT_RES_FINISH;
		}
	}
}

bool HTTP::resSendSize(size_t & remain, std::string & str) {
	size_t len;

	len = str.size();
	std::cout << "[[" << str << "]]" << std::endl;
	if (len > remain) {
		write(socket_fd, str.c_str(), remain);
		str.substr(remain);
		remain = 0;
		return false;
	}
	else {
		write(socket_fd, str.c_str(), len);
		remain = remain - len;
		return true;
	}
}

bool HTTP::resSendFD(size_t & remain) {
	char buf[remain];
	size_t len;

	len = read(response_fd, buf, remain);
	std::cout << "[[[" << buf << "]]]" << std::endl;
	write(socket_fd, buf, len);
	if (len == remain)
		return false;
	else
		return true;
}

/*
** response message가 완료되었나 체크하는 함수
** true : 완료, false : 미완료
*/
bool HTTP::resCheckFinished() {
	if (responseMessage.step == CLIENT_RES_FINISH)
		return true;
	else
		return false;
}
