#ifndef HTTP_HPP
#define HTTP_HPP

#include <map>
#include <iostream>
#include <unistd.h>
// #include <sstream>
#include "../config/Config.hpp"
#include "HTTP_Chunk.hpp"
#include "../CGInterface/CGInterface.hpp"

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
#define PUT                     "PUT"

typedef std::map<std::string, std::string> HTTPHeaderField;

struct  RequestMessage {
	/* raw data */
	std::string             buf;           // storage of whole request message;

	/* request_line */
	std::string             request_line;   //  original request line
	std::string             unparsed_uri;   //  original uri
	std::string             method_name;   	//	method name string
	std::string             http_version;
	char					method;			//	method bit value

	/* request header */
	HTTPHeaderField         header_in;
	unsigned int			port_num;
	bool					keep_alive;

	/* request body */
	std::string             body;
	Chunk                   chunk;

	bool                    non_body;
	long                    content_length;
	bool                    chunked;
	int                     request_step;
};

struct ResponseMessage {
	int						server_block_index;
	int						location_index;
	bool					have_file_fd;
	std::string             file_directory;
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
	unsigned long	req_finished_time;
	CGInterface		*cgi;

	/* request function */
	bool	isReadyRequestLine();
	bool	isReadyRequestHeader();
	bool	isReadyRequestBody();
	void	parseRequestLine();
	void	parseRequestHeader();
	void	additionalParseRequestHeader();
	bool	parseRequestBody();
	int     reqBodyChunked();
	int     reqBodyContentLength();
	void    addHeader(std::pair<std::string, std::string> & header);
	/* utils */
	char    methodStringtoBit(std::string str);
	bool    extractstr(std::string & dest, std::string & src, std::string const & cut);
	bool    extractstr(std::string & dest, std::string & src, size_t len);

public:
	HTTP();
	HTTP(uintptr_t _server_fd);
	void					resetHTTP();
	~HTTP();

	uintptr_t const &		getServerFd() const;
	std::string const & 	getURI() const;
	void setURI(std::string const & str);
	std::string & 			getBody();
	int const &				getStatus();
	unsigned int const &	getPort();
	char				 	getMethod() const;
	bool const &			getKeepAlive() const;

	void					setPort();
	void					setStatus(int const & s);

	/* request function */
	void	reqInputBuf(std::string const & str);
	void	reqPrint();
	bool	reqCheckFinished();
	bool	reqCheckHeaderFinished();

	/* response function */
	std::string const &		getResponseFileDirectory();
	std::string const & 	getResponseLine() const;
	std::string const & 	getResponseHeader() const;
	std::string const & 	getResponseBody() const;
	bool const &			getResponseHaveFileFd() const;

	void					setResponseFileDirectory(std::string const & str);
	void					setResponseLine();
	void					setResponseHeader(std::string const & key, std::string const & value);
	void					setResponseBody(std::string const & str);
	void 					setResponseHaveFileFd(bool const & have);
	
	bool					checkStatusError();
	unsigned long const &	getReqFinishedTime();
	int const & 			getResServerBlockIndex();
	void					setResServerBlockIndex(int const & i);
	int const & 			getResLocationIndex();
	void					setResLocationIndex(int const & i);

	/*  CGI function  */
	void					cgi_creat(uintptr_t &write_fd, uintptr_t &read_fd, pid_t &pid);
	bool					cgi_write(size_t buf_size);
	bool					cgi_read(size_t buf_size);

	// uintptr_t const & getResponseFd();
	// void setResponseFd(uintptr_t const & s);
};

#endif
