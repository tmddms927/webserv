#include "HTTP.hpp"
#include "../CGIInterface/CGIInterface.hpp"
#include "../utils.hpp"
#include <iostream>
#include "../Server/Server_define.hpp"

void    HTTP::cgi_creat(uintptr_t &write_fd, uintptr_t &read_fd, pid_t &pid) {
    struct s_cgiInfo    ci;
    struct s_cgiArg     ca;

    requestMessage.buf = "";
    // cgi.CGI_clear();
    ca.content_length = requestMessage.body.size();
    ca.method_name = requestMessage.method_name;

    cgi.CGI_fork(ci, ca);
    write_fd = ci.write_fd;
    read_fd = ci.read_fd;
    pid = ci.pid;
}

int    HTTP::cgi_write(size_t buf_size) {
    std::string buf;
    int         cgi_wr;

    if (cgi.CGI_getWrittenLength() == requestMessage.body.size()) {
        close(cgi.CGI_getWriteFd());
        return 1;
    }
    buf = requestMessage.body.substr(cgi.CGI_getWrittenLength(), buf_size);
    cgi_wr = cgi.CGI_write(buf);
    if (cgi_wr == 0) {
        std::cout << "CGI_write error" << std::endl;
        return 0;
    }
    if (cgi_wr == -1) {
        std::cout << "CGI_write pipe buf error" << std::endl;
        return -1;
    }
    return -2;
}

int    HTTP::cgi_read(size_t buf_size) {
    int         cgi_rd;
    std::string tmp;

    cgi_rd = cgi.CGI_read(tmp);
    if (cgi_rd == 0) {
        std::cout << "CGI_read error" << std::endl;
        return 0;
    }
    if (cgi_rd == -1) {
        std::cout << "CGI_read pipe buf error" << std::endl;
        return -1;
    }

    if (status <= 0) {
        std::cout << "header parsing~" << std::endl;
        requestMessage.buf += tmp;
        // std::cout << "[" << requestMessage.buf << "]" << std::endl;
        tmp = "";
        if (extractstr(tmp, requestMessage.buf, "\r\n\r\n")) {
            std::vector<std::string> v;

            ft_split(v,tmp, "\r\n");
            for (std::vector<std::string>::iterator
                    it = v.begin(); it != v.end(); it++) {
                        std::cout << *it << std::endl;
                        if (it == v.begin()) {
                            setStatus(200);
                            setResponseLine();
                        }
                        else {
                            std::pair<std::string, std::string> pa;
                            pa = ft_slice_str(*it, it->find(":"));
                            setResponseHeader(pa.first, pa.second);
                        }
                    }
            if (!requestMessage.buf.empty())
                setResponseBody(requestMessage.buf);
        }
        else
            return false;
    }
    else 
        setResponseBody(tmp);
    std::cout << getResponseBody().size() << ", " << requestMessage.body.size() << std::endl;
    if (getResponseBody().size() < requestMessage.body.size())
        return false;
    setResponseHeader("Content-Length", ft_itoa(requestMessage.body.size()));
    close(cgi.CGI_getReadFd());
    return CGI_READ_FINSHED;
}

