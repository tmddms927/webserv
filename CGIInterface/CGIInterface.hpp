#ifndef CGIInterface_HPP
#define CGIInterface_HPP

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

struct s_cgiArg {
    std::string cgi_path;
    size_t      content_length; // if body exist
    std::string method_name;
    std::string http_protocol;
    std::string path_info;
};

class CGIInterface
{
private:
    /*  fds  */

    /* util */
    size_t      written_length;
    int   read_fd;
    int   write_fd;
    // CGIInterface(CGIInterface const &);
    // CGIInterface & operator=(CGIInterface const &);
public:
    CGIInterface();
    ~CGIInterface();

    void    CGI_fork(struct s_cgiInfo &ci, struct s_cgiArg &ca);
    int     CGI_write(std::string const &body);
    int     CGI_read(std::string & buf);
    size_t  CGI_getWrittenLength();
    void    CGI_clear();
    int     CGI_getReadFd();
    int     CGI_getWriteFd();
};

// CGIInterface::CGIInterface(/* args */)
// {
// }

// CGIInterface::~CGIInterface()
// {
// }


#endif