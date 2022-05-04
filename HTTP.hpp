#ifndef HTTP_HPP
#define HTTP_HPP

#include <map>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "Config.hpp"

/* new phase level */
#define PRE_READ_REQUEST_HEADER 0
#define POST_READ_REQUEST_HEADER 1


/* reqParsing */
#define REQ_REQUEST_LINE        0
#define REQ_HEADER_FIELD        1

/* process */
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

struct  Chunk {
    size_t                  length;
    std::string             content;
};

struct  RequestMessage {
    /* raw data */
    std::string             buf;           // storage of whole request message;
    /* raw data */

    /* request_line */
    std::string             request_line;   //  original request line
    std::string             unparsed_uri;   //  original uri
    std::string             method;         // -> method name: method name string
    //int                   method;         //  method's numeric value defined
    std::string             path;           // -> uri : parsed uri(deferent with unparsed_uri)
    /* request_line */

    /* request header */
    HTTPHeaderField         header_in;
    /* request header */

    /* request body */
    std::string             body;
    Chunk                   chunk;
    /* request body */


    long                   content_length;
//    std::string             buf;
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
    const servers   server;
    uintptr_t       socket_fd;
    RequestMessage  requestMessage;
    ResponseMessage responseMessage;
    int             status;
    int             protocol_minor_version;
public:
    HTTP();
    HTTP(servers const & _server, uintptr_t _socket_fd);

    /* request function */
    void reqInputBuf(std::string const & str);
    bool reqbufCheck();
    void reqParsing();
    void reqParsingRequestLine(std::string & temp);
    void reqParsingHeaderField(std::string const & temp);

    int     process_request_headers(std::string const &req_header_field);
    int     process_request_body(std::string const &req_header_field);
    void    content_phase();

    void reqBodyContentLength(std::string const & temp);
    void reqBodyChunked(std::string const & temp);
    bool reqCheckFinished();
    void reqPrint();
    void reqChunkInit();

    /* valid */
    void reqGETHeaderCheck();
    void reqPOSTHeaderCheck();
    void bodyEncodingType();

    /* response function */
    void resSendMessage();
};

#endif
