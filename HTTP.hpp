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

#define CLIENT_RES_LINE         10
#define CLIENT_RES_HEADER       11
#define CLIENT_RES_BODY         12
#define CLIENT_RES_FINISH       13

#define BAD_REQUEST             400
#define NOT_ALLOWED             405
#define PATLOAD_TOO_LARGE       413

#define GET                     "GET"
#define POST                    "POST"
#define DELETE                  "DELETE"
#define HEAD                    "HEAD"

typedef std::map<std::string, std::string> HTTPHeaderField;

struct  Chunk {
    long                    length;
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
    int                     request_step;
};

struct ResponseMessage {
    std::string             file_directory;
    int                     step;
    std::string             response_line;
    std::string             header;
    std::string             body;
};

class HTTP {
private:
    uintptr_t       server_fd;
    RequestMessage  requestMessage;
    ResponseMessage responseMessage;
    int             status;
    uintptr_t       response_fd;

    int             process_request_line();
    int             process_request_headers();
    int             process_request_body();
    int             set_body_parsor();
    void            content_phase();

    int             reqBodyContentLength();
    int             reqBodyChunked();

public:
    HTTP();
    HTTP(uintptr_t _server_fd);
    uintptr_t const & getServerFd() const;
    std::string const & getMethod() const;
    std::string const & getURI() const;
    std::string & getBody();
    int const & getStatus();
    uintptr_t const & getResponseFd();
    void setResponseFd(uintptr_t const & s);
    void setStatus(int const & s);
    std::string const & getResponseLine();
    std::string const & getResponseHeader();
    std::string const & getResponseBody();

    void resetHTTP();

    /* request function */
    void reqInputBuf(std::string const & str);

    /* */
    void reqPrint();
    bool reqCheckFinished();
    void reqChunkInit();


    /* response function */
    void setResponseFileDirectory(std::string const & str);
    void setResponseLine();
    void setGETHeader();
    void setPOSTHeader();
    void setResponseBody(std::string const & str);
    void setErrorResponse();
};




#endif
