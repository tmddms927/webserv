#ifndef HTTP_HPP
#define HTTP_HPP

#include <map>
#include <iostream>
#include <unistd.h>

#define REQ_REQUEST_LINE        0
#define REQ_HEADER_FIELD        1
#define REQ_BODY                2
#define REQ_FINISHED            3

typedef std::map<std::string, std::string> HTTPHeaderField;

struct  RequestMessage {
    // 공통으로 씀
    std::string             method;
    // 공통으로 씀..?
    std::string             path;
    HTTPHeaderField         header;
    std::string             body;
    // 필요없는거 같음
    // long                    length;
    std::string             buf;
    int                     err_num;
    // 공통으로 씀..?
    int                     current;
};

struct ResponseMessage {
    // header field 저장을 request와 같이 map으로 변경
    // HTTPHeaderField         header;
    std::string header;
    std::string body;
};

class HTTP {
private:
    uintptr_t       socket_fd;
    RequestMessage  requestMessage;
    ResponseMessage responseMessage;
    int             status;
    int             protocol_minor_version;
public:
    HTTP();
    HTTP(uintptr_t _socket_fd);

    /* request function */
    void reqInputBuf(std::string const & str);
    bool reqbufCheck();
    void reqParsing();
    void reqParsingRequestLine(std::string & temp);
    void reqParsingHeaderField(std::string const & temp);
    void reqParsingBody(std::string const & temp);
    bool reqCheckFinished();
    void reqPrint();

    /* response function */
    void resSendMessage();
};

#endif
