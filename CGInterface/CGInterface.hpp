#ifndef CGINTERFACE_HPP
#define CGINTERFACE_HPP

#include <unistd.h>
#include <vector>

#define CGI_READ_RESPOSE_STATUS_LINE    0
#define CGI_READ_RESPOSE_HEADER         1
#define CGI_READ_RESPOSE_BODY           2

typedef std::map<std::string, std::string> CGIHeaderMap;

struct s_cgiInfo {
    uintptr_t   write_fd;
    uintptr_t   read_fd;
    pid_t       pid;   
};

struct cgi_struct {
    std::string cgi_path;
    size_t      content_length; // if body exist
    std::string method_name;
    std::string http_protocol;
    std::string path_info;
};

class CGInterface
{
private:
    /* must initiated */
    size_t      cgi_write_len;
    size_t      written_len;
    size_t      read_len;

    /*  fds  */
    uintptr_t   write_fd;
    uintptr_t   read_fd;

    /*      */

public:
    CGInterface(/* args */);
    ~CGInterface();

    void    CGI_fork(struct s_cgiInfo *ci);
    bool    CGI_write(std::string const &body);
    bool    CGI_read(std::string & buf, size_t buf_size);
    size_t  CGI_getWrittenLength();
};

// CGInterface::CGInterface(/* args */)
// {
// }

// CGInterface::~CGInterface()
// {
// }


#endif