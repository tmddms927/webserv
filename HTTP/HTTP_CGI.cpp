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

int    HTTP::cgi_write() {
    std::string buf;
    int         cgi_wr;

    if (cgi.CGI_getWrittenLength() == requestMessage.body.size()) {
        close(cgi.CGI_getWriteFd());
        return CGI_FINISHED;
    }
    buf = requestMessage.body.substr(cgi.CGI_getWrittenLength(), RW_MAX_SIZE);
    cgi_wr = cgi.CGI_write(buf);
    if (cgi_wr == 0) {
        std::cout << "CGI_write error" << std::endl;
        return CGI_ERROR;
    }
    // -1 is not error, pipe 버퍼가 차 있으므로 다시 write 시도하면 됨
    return CGI_NOT_FINISHED;
}

int    HTTP::cgi_read() {
    int         cgi_rd;
    std::string tmp;

    cgi_rd = cgi.CGI_read(tmp);
    if (cgi_rd == 0)
        return CGI_ERROR;
    // -1 is not error, pipe 버퍼가 차 있으므로 다시 read 시도하면 됨
    // 만약 CGI가 시간 내에 요청을 처리하지 못하면 timeout
    if (cgi_rd == -1)
        return CGI_NOT_FINISHED;

    cgi_buf = tmp;
    if (cgi_header_buf.empty()) {
        if (!extractstr(cgi_header_buf, cgi_buf, "\r\n\r\n"))
            return CGI_NOT_FINISHED;
        else
            setResponseBody(cgi_buf);
    }
    if (getResponseBody().size() < requestMessage.body.size()) {
        return CGI_NOT_FINISHED;
    }
    //  getResponseBody().size() == requestMessage.body.size() 인경 read finish
    close(cgi.CGI_getReadFd());
    return CGI_FINISHED;
}

int     HTTP::cgi_setResponseline() {
    size_t      found;
    std::string cgi_response_line;
    std::vector<std::string> tmp_v;

    if (cgi_header_buf.empty())
        return CGI_ERROR;
    found = cgi_header_buf.find("\r\n");
    if (found == std::string::npos)
        return CGI_ERROR;
    cgi_response_line = cgi_header_buf.substr(0, found);
    ft_split(tmp_v, cgi_response_line, " ");
    if (tmp_v.size() != 3)
        return CGI_ERROR;
    setStatus(std::strtod(tmp_v[1].c_str(), 0));
    setResponseLine();
    return CGI_FINISHED;
}

int    HTTP::cgi_setResponseHeader() {
    std::vector<std::string> cgi_reponse_header;

    ft_split(cgi_reponse_header, cgi_header_buf, "\r\n");
    for (std::vector<std::string>::iterator 
            it = cgi_reponse_header.begin() + 1; it != cgi_reponse_header.end(); it++) {
                std::pair<std::string, std::string> tmp_p;
                size_t found = it->find(":");

                if (found == std::string::npos)
                    return CGI_ERROR;
                tmp_p = ft_slice_str(*it, found);
                ft_trim_space(tmp_p.first);
                ft_trim_space(tmp_p.second);
                setResponseHeader(tmp_p.first, tmp_p.second);
            }

    if (getResponseBody().size())
        setResponseHeader("Content-Length", ft_itoa(getResponseBody().size()));
    return CGI_FINISHED;
}

    // if (status <= 0) {
    //     std::cout << "header parsing~" << std::endl;
    //     requestMessage.buf += tmp;
    //     // std::cout << "[" << requestMessage.buf << "]" << std::endl;
    //     tmp = "";
    //     if (extractstr(tmp, requestMessage.buf, "\r\n\r\n")) {
    //         std::vector<std::string> v;

    //         ft_split(v,tmp, "\r\n");
    //         for (std::vector<std::string>::iterator
    //                 it = v.begin(); it != v.end(); it++) {
    //                     std::cout << *it << std::endl;
    //                     if (it == v.begin()) {
    //                         setStatus(200);
    //                         setResponseLine();
    //                     }
    //                     else {
    //                         std::pair<std::string, std::string> pa;
    //                         pa = ft_slice_str(*it, it->find(":"));
    //                         setResponseHeader(pa.first, pa.second);
    //                     }
    //                 }
    //         if (!requestMessage.buf.empty())
    //             setResponseBody(requestMessage.buf);
    //     }
    //     else
    //         return false;
    // }
    // setResponseHeader("Content-Length", ft_itoa(requestMessage.body.size()));

