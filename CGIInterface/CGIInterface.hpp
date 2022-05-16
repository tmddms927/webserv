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

class CGIInterface
{
private:
    size_t      written_length;
    int   read_fd;
    int   write_fd;
    // CGIInterface(CGIInterface const &);
    // CGIInterface & operator=(CGIInterface const &);
public:
    CGIInterface();
    ~CGIInterface();

    int     CGI_fork(struct s_cgiInfo & ci, 
                        std::vector<std::string> & arg, 
                        std::vector<std::string> & env);
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