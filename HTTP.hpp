#ifndef HTTP_HPP
#define HTTP_HPP

#include <map>
#include <iostream>
#include <unistd.h>

#define REQ_REQUEST_LINE        0
#define REQ_HEADER_FIELD        1
#define REQ_BODY                2
#define REQ_CONTENT_LENGTH      3
#define REQ_CHUNKED             4
#define REQ_FINISHED            5

#define BAD_REQUEST             -1
#define NO_BODY                 -2

#define GET                     "GET"
#define POST                    "POST"
#define DELETE                  "DELETE"

typedef std::map<std::string, std::string> HTTPHeaderField;

struct  RequestMessage {
    // 공통으로 씀
    std::string             method;
    // 공통으로 씀..?
    std::string             path;
    HTTPHeaderField         header;
    std::string             body;
    //long                   content_length;
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
    void reqBodyContentLength(std::string const & temp);
    void reqBodyChunked(std::string const & temp);
    bool reqCheckFinished();
    void reqPrint();

    /* valid */
    void reqGETHeaderCheck();
    void bodyEncodingType();

    /* response function */
    void resSendMessage();
};

#endif
