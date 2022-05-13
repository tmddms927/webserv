//
// Created by Giyoung Lee on 5/12/22.
//

#ifndef UNTITLED1_AUTOINDEX_HPP
#define UNTITLED1_AUTOINDEX_HPP

#include "../HTTP/HTTP.hpp"
#include "../Server/Server.hpp"

/*

struct  RequestMessage {
	 raw data
std::string             buf;           // storage of whole request message;

 request_line
std::string             request_line;   //  original request line
std::string             unparsed_uri;   //  original uri
std::string             method_name;   	//	method name string
std::string             http_version;
char					method;			//	method bit value

 request header
HTTPHeaderField         header_in;
unsigned int			port_num;
bool					keep_alive;

 request body
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

*/
class AutoIndex {
private:
    ResponseMessage                           _res;
    std::vector<servers>                   _config;
public:
    AutoIndex(std::vector<servers> const & config);
    void                                makeHTML();
};


#endif //UNTITLED1_AUTOINDEX_HPP
