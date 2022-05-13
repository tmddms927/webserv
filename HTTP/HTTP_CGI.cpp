#include "HTTP.hpp"
#include "../CGInterface/CGInterface.hpp"
#include "../utils.hpp"
#include <iostream>

void    HTTP::cgi_creat(uintptr_t &write_fd, uintptr_t &read_fd, pid_t &pid) {
    struct s_cgiInfo    ci;
    struct s_cgiArg     ca;

    requestMessage.buf = "";
    if (cgi)
        std::cout << "CGI already exist in HTTP instance" << std::endl;
    cgi = new CGInterface();

    ca.content_length = requestMessage.body.size();
    ca.method_name = requestMessage.method_name;

    cgi->CGI_fork(ci, ca);
    write_fd = ci.write_fd;
    read_fd = ci.read_fd;
    pid = ci.pid;
}

bool    HTTP::cgi_write(size_t buf_size) {
    std::string buf;
    int         cgi_wr;

    buf = requestMessage.body.substr(cgi->CGI_getWrittenLength(), buf_size);
    if (buf.empty())
        return true;
    cgi_wr = cgi->CGI_write(buf);
    if (cgi_wr < 0) {
        std::cout << "HTTP cgi_write have no write to buf" << std::endl;
        exit(1);
    }
    if (cgi == 0)
        std::cout << "HTTP cgi_write fd have some problem";
    if (cgi->CGI_getWrittenLength() >= requestMessage.body.size()) {
        close(cgi->write_fd);
        return true;
    }
    return false;
}

bool    HTTP::cgi_read(size_t buf_size) {
    int         cgi_rd;
    std::string tmp;

    std::cout << "cgi_read" << std::endl;
    cgi_rd = cgi->CGI_read(tmp, buf_size);
        // return false;
    
    // setResponseHeader("Host: )
    if (status <= 0) {
        std::cout << "header parsing~" << std::endl;
        requestMessage.buf += tmp;
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
    setResponseHeader("Content-Length: ", ft_itoa(requestMessage.body.size()));
    close(cgi->read_fd);
    return true;
}

