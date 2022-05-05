#ifndef HTTP_HPP
#define HTTP_HPP

#include <map>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "Config.hpp"

/* new phase level */
#define CLIENT_READ_REQ_LINE 0
#define CLIENT_READ_REQ_HEADER 1
#define CLIENT_READ_REQ_BODY 2
#define CLIENT_READ_FINISH 3


/* status */
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
    int (RequestMessage::*body_parsor)(void);
    std::string             body;
    Chunk                   chunk;
    /* request body */

    bool                    non_body;
    long                    content_length;
    bool                    chunked;
//    std::string             buf;
    int                     err_num;
    // 공통으로 씀..?
    int                     current;

public:
    int reqBodyContentLength();
    int reqBodyChunked();

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
    // bool reqbufCheck();
    // void reqParsing();
    // void reqParsingHeaderField(std::string const & temp);

    int     process_request_line();
    int     process_request_headers();
    int     process_request_body();
    int     set_body_parsor();
    void    content_phase();

    // int reqBodyContentLength();
    // int reqBodyChunked();
    bool reqCheckFinished();
    void reqPrint();
    // void reqChunkInit();

    /* valid */
    // void reqGETHeaderCheck();
    // void reqPOSTHeaderCheck();
    // void bodyEncodingType();

    /* response function */
    void resSendMessage();


    std::string const & getMethod() const;
    std::string const & getURI() const;
    std::string & getBody();
};




#endif
