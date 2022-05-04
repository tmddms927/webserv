#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

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

    // todo write fd error check!
    write(socket_fd, responseMessage.header.c_str(), responseMessage.header.size());
    write(socket_fd, "\r\n", 2);
    write(socket_fd, responseMessage.body.c_str(), responseMessage.body.size());
    write(socket_fd, "\r\n", 2);
    requestMessage.current = REQ_REQUEST_LINE;
}
