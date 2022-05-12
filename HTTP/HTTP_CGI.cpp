#include "HTTP.hpp"
#include "CGInterface.hpp"
#include <iostream>

void    HTTP::cgi_creat(uintptr_t *write_fd, uintptr_t *read_fd, pid_t *pid) {
    struct s_cgiInfo    ci;

    requestMessage.buf = "";
    if (cgi)
        std::cout << "CGI already exist in HTTP instance" << std::endl;
    cgi = new CGInterface();
    cgi->CGI_fork(&ci);
    *write_fd = ci.write_fd;
    *read_fd = ci.read_fd;
    *pid = ci.pid;
}

bool    HTTP::cgi_write(size_t buf_size) {
    std::string buf;
    buf = requestMessage.body.substr(cgi->CGI_getWrittenLength(), buf_size);
    if (cgi->CGI_write(buf)) // true면 finish
        return true;
    return false;
}

bool    HTTP::cgi_read(size_t buf_size) {
    if (!cgi->CGI_read(requestMessage.buf, buf_size))
        return false;
    //setSatatus;
    // setResponseLine();
    // for cgi->getHeader().size()
        // setResponseHeader(it->first, it->second);
    // setResponseBody(cgi->getBody());
    return true;
}
