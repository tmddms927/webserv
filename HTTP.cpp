#include "HTTP.hpp"

/*
** HTTP class default constructor.
*/
HTTP::HTTP() : socket_fd(-1) {

}

/*
** HTTP class constructor. socket_fd값을 받아와서 저장.
*/
HTTP::HTTP(uintptr_t _socket_fd) : socket_fd(_socket_fd) {
}

/*
** request message가 왔을 경우 request.buf에 메시지 저장
*/
void HTTP::reqInputBuf(std::string const & str) {
    requestMessage.buf += str;
    if (reqbufCheck() == true) 
        reqParsing();
    reqPrint();
    // request message 끝나면
    if (reqCheckFinished() == true) {
        std::cout << socket_fd << std::endl;
        resSendMessage();
    }
}

/*
** request message에 "\r\n"이 있는지 체크
*/
bool HTTP::reqbufCheck() {
    if (requestMessage.buf.find("\r\n\r\n") == std::string::npos)
        return false;
    return true;
}

/*
** parsing 시작하는 함수.
** requestline, header field, body인지 구분하여 해당 함수 실행
*/
void HTTP::reqParsing() {
    size_t index;
    std::string temp;

    while (1) {
        index = requestMessage.buf.find("\r\n");
        if (index == std::string::npos)
            break;
        temp = requestMessage.buf.substr(0, index);
        if (requestMessage.current == REQ_REQUEST_LINE)
            reqParsingRequestLine(temp);
        else if (requestMessage.current == REQ_HEADER_FIELD)
            reqParsingHeaderField(temp);
        else if (requestMessage.current == REQ_BODY) {
            reqParsingBody(temp);
        }
        requestMessage.buf = requestMessage.buf.substr(index + 1);
    }
}

/*
** request message - request line를 파싱하는 함수
*/
void HTTP::reqParsingRequestLine(std::string & temp) {
    std::size_t index;

    // method
    index = temp.find(' ');
    if (index == std::string::npos)
        std::cout << "method error!" << index << '\n';
    requestMessage.method = temp.substr(0, index);
    temp = temp.substr(index + 1);

    // request-uri
    index = temp.find(' ');
    if (index == std::string::npos)
        std::cout << "request-uri error!" << index << '\n';
    requestMessage.path = temp.substr(0,index);
    temp = temp.substr(index + 1);

    // HTTP-version
    if (temp != "HTTP/1.1\r\n")
        std::cout << "HTTP-version error!" << index << '\n';
    protocol_minor_version = 1;
    requestMessage.current = REQ_HEADER_FIELD;
}

/*
** request message - header field를 파싱하는 함수
*/
void HTTP::reqParsingHeaderField(std::string const & temp) {
    size_t index;
    std::string first;
    std::string second;

    if (temp == "\r\n") {
        requestMessage.current = REQ_BODY;
        return ;
    }
    index = temp.find(":");
    if (!index)
        std::cout << "HeaderField [key:value] error. there are no key!" << std::endl;
    //todo !need to modify key and value validation check!
    first = temp.substr(0, index - 1);
    second = temp.substr(index + 1);
    requestMessage.header.insert(std::pair<std::string, std::string>(first, second));
}

/*
** request message - body를 파싱하는 함수
*/
void HTTP::reqParsingBody(std::string const & temp) {
    if (temp == "\r\n") {
        requestMessage.current = REQ_FINISHED;
    }
    requestMessage.body += temp;
}

/*
** request message가 완료되었나 체크하는 함수
** true : 완료, false : 미완료
*/
bool HTTP::reqCheckFinished() {
	if (requestMessage.current == REQ_FINISHED)
		return true;
	else
		return false;
}

/*
** request message print
*/
void HTTP::reqPrint() {
    std::cout << "message : " << requestMessage.method << std::endl;
    std::cout << "path : " << requestMessage.path << std::endl;
    for (std::map<std::string, std::string>::iterator it = requestMessage.header.begin(); it != requestMessage.header.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
    std::cout << "body : " << requestMessage.body << std::endl;
}

/*
** response message를 socket fd에 write하는 함수
*/
void HTTP::resSendMessage() {
    responseMessage.header = "HTTP/1.1 200 OK\r\n"
             "Server: nginx/1.21.6\r\n"
             "Date: Tue, 26 Apr 2022 10:59:45 GMT\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: 615\r\n"
             "Last-Modified: Tue, 25 Jan 2022 15:03:52 GMT\r\n"
             "Connection: keep-alive\r\n"
             "ETag: \"61f01158-267\"\r\n"
             "Accept-Ranges: bytes\r\n";
    responseMessage.body = "<!DOCTYPE html>\r\n"
           "<html>\r\n"
           "<head>\r\n"
           "<title>Welcome to nginx!</title>\r\n"
           "<style>\r\n"
           "html { color-scheme: light dark; }\r\n"
           "body { width: 35em; margin: 0 auto;\r\n"
           "font-family: Tahoma, Verdana, Arial, sans-serif; }\r\n"
           "</style>\r\n"
           "</head>\r\n"
           "<body>\r\n"
           "<h1>Welcome to nginx!</h1>\r\n"
           "<p>If you see this page, the nginx web server is successfully installed and\r\n"
           "working. Further configuration is required.</p>\r\n"
           "\r\n"
           "<p>For online documentation and support please refer to\r\n"
           "<a href=\"http://nginx.org/\">nginx.org</a>.<br/>\r\n"
           "Commercial support is available at\r\n"
           "<a href=\"http://nginx.com/\">nginx.com</a>.</p>\r\n"
           "\r\n"
           "<p><em>Thank you for using nginx.</em></p>\r\n"
           "</body>\r\n"
           "</html>\r\n";
    
    // write fd error check!
    write(socket_fd, responseMessage.header.c_str(), responseMessage.header.size());
    write(socket_fd, "\r\n", 2);
    write(socket_fd, responseMessage.body.c_str(), responseMessage.body.size());
    write(socket_fd, "\r\n", 2);
}
