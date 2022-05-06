#ifndef HTTP_HPP
#define HTTP_HPP

#include <map>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "config/Config.hpp"

/* new phase level */
#define CLIENT_READ_REQ_LINE 0
#define CLIENT_READ_REQ_HEADER 1
#define CLIENT_READ_REQ_BODY 2
#define CLIENT_READ_FINISH 3

#define CLIENT_RES_LINE         10
#define CLIENT_RES_HEADER       11
#define CLIENT_RES_BODY         12
#define CLIENT_RES_FINISH       13

/* status */
#define BAD_REQUEST             -1
#define NO_BODY                 -2

#define GET                     "GET"
#define POST                    "POST"
#define DELETE                  "DELETE"

typedef std::map<std::string, std::string> HTTPHeaderField;

struct  Chunk {
    long                    length;
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

    bool                    non_body;
    long                    content_length;
    bool                    chunked;
    int                     current;
};

struct ResponseMessage {
    int                     step;
    std::string             response_line;
    std::string             header;
    std::string             body;
};

class HTTP {
private:
    const servers   server;
    uintptr_t       socket_fd; // 필요없는 것 같음
    RequestMessage  requestMessage;
    ResponseMessage responseMessage;
    int             status;
    int             protocol_minor_version;

    uintptr_t             response_fd;

    int             process_request_line();
    int             process_request_headers();
    int             process_request_body();
    int             set_body_parsor();
    void            content_phase();

    int             reqBodyContentLength();
    int             reqBodyChunked();

public:
    HTTP();
    HTTP(servers const & _server, uintptr_t _socket_fd);
    std::string const & getMethod() const;
    std::string const & getURI() const;
    std::string & getBody();
    int const & getStatus();
    uintptr_t const & getResponseFd();
    void setResponseFd(uintptr_t const & s);
    void setStatus(int const & s);

    void resetHTTP();

    /* request function */
    void reqInputBuf(std::string const & str);

    /* */
    void reqPrint();
    bool reqCheckFinished();


    // int reqBodyContentLength();
    // int reqBodyChunked();
    // void reqChunkInit();

    /* valid */
    // void reqGETHeaderCheck();
    // void reqPOSTHeaderCheck();
    // void bodyEncodingType();

    /* response function */
    void setResponseLine();
    void setGETHeader();
    void setPOSTHeader();
    void setErrorResponse();
    void resSendMessage();
    bool resSendSize(size_t & remain, std::string & str);
    bool resSendFD(size_t & remain);
    bool resCheckFinished();



    // bool reqbufCheck();
    // void reqParsing();
    // void reqParsingHeaderField(std::string const & temp);
};




#endif
