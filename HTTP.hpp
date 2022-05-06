#ifndef HTTP_HPP
#define HTTP_HPP

#include <map>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "config/Config.hpp"

#define ERROR   -1
#define SUCCESS 0
#define FAIL    1

/* new phase level */
#define CLIENT_READ_REQ_LINE 0
#define CLIENT_READ_REQ_HEADER 1
#define CLIENT_READ_REQ_BODY 2
#define CLIENT_READ_FINISH 3

/* status */
#define BAD_REQUEST             400
#define NOT_ALLOWED             405
#define PATLOAD_TOO_LARGE       413

#define GET                     "GET"
#define POST                    "POST"
#define DELETE                  "DELETE"

typedef std::map<std::string, std::string> HTTPHeaderField;

struct  Chunk {
    size_t                  length;
    std::string             content;
};

struct  RequestMessage {
    /* raw data */
    std::string             buf;           // storage of whole request message;

    /* request_line */
    std::string             request_line;   //  original request line
    std::string             unparsed_uri;   //  original uri
    std::string             method;         // -> method name: method name string
    std::string             http_version;
    std::string             path;           // -> uri : parsed uri(deferent with unparsed_uri)

    /* request header */
    HTTPHeaderField         header_in;

    /* request body */
    std::string             body;
    Chunk                   chunk;

    bool                    non_body;
    long                    content_length;
    bool                    chunked;
//    std::string             buf;
    int                     err_num;
    // 공통으로 씀..?
    int                     request_step;
};

struct ResponseMessage {
    // header field 저장을 request와 같이 map으로 변경
    // HTTPHeaderField         header;
    std::string header;
    std::string body;
};

class HTTP {
private:
    const servers   server;
    uintptr_t       socket_fd; // 필요없는 것 같음
    RequestMessage  requestMessage;
    ResponseMessage responseMessage;
    int             status;

    int     process_request_line();
    int     process_request_headers();
    int     process_request_body();
    int     set_body_parsor();
    void    content_phase();

    int     reqBodyContentLength();
    int     reqBodyChunked();

public:
    HTTP();
    HTTP(servers const & _server, uintptr_t _socket_fd);

    /* request function */
    void reqInputBuf(std::string const & str);

    /* getter */
    std::string const & getMethod() const;
    std::string const & getURI() const;
    std::string & getBody();

    /* */
    void reqPrint();
    bool reqCheckFinished();


    // int reqBodyContentLength();
    // int reqBodyChunked();
    void reqChunkInit();

    /* valid */
    // void reqGETHeaderCheck();
    // void reqPOSTHeaderCheck();
    // void bodyEncodingType();

    /* response function */
    void resSendMessage();



    // bool reqbufCheck();
    // void reqParsing();
    // void reqParsingHeaderField(std::string const & temp);
};




#endif
