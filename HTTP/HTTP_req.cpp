#include "HTTP.hpp"
#include "../utils.hpp"
#include "../Server/Server.hpp"
#include "HTTPHeaderField.hpp"
#include <vector>
#include <utility>

void    HTTP::addHeader(std::pair<std::string, std::string> & header) {
    ft_trim_space(header.first);
    ft_trim_space(header.second);
    requestMessage.header_in.insert(header);
    //header validate
}

bool    HTTP::extractstr(std::string & dest, std::string & src, std::string const & cut) {
    size_t found;

    found = src.find(cut);
    if (found == std::string::npos)
        return false;
    dest = src.substr(0, found);
    if (dest.size() + cut.size() == src.size())
        src = "";
    else
        src = src.substr(found + cut.size());
    return true;
}

bool    HTTP::extractstr(std::string & dest, std::string & src, size_t len) {
    dest = src.substr(0, len);
    if (dest.size() == src.size())
        src = "";
    else
        src = src.substr(len + 1);
    return true;
}

bool    HTTP::isReadyRequestLine() {
    if (requestMessage.request_step == CLIENT_READ_REQ_LINE &&
        requestMessage.buf.find("\r\n") != std::string::npos)
        return true;
    return false;
}

bool    HTTP::isReadyRequestHeader() {
    if (requestMessage.request_step == CLIENT_READ_REQ_HEADER &&
        requestMessage.buf.find("\r\n\r\n") != std::string::npos)
        return true;
    return false;
}

bool    HTTP::isReadyRequestBody() {
    if (requestMessage.request_step == CLIENT_READ_REQ_BODY)
        return true;
    return false;
}

void    HTTP::parseRequestLine() {
    std::string line;
    std::vector<std::string> v;
    extractstr(line, requestMessage.buf, "\r\n");
    ft_trim_space(line);
    ft_split(v, line, " ");
    if (v.size() != 3)
        setStatus(BAD_REQUEST);
    requestMessage.method_name = v[0];
    requestMessage.unparsed_uri = v[1];
    requestMessage.http_version = v[2];
}

void    HTTP::parseRequestHeader() {
    std::string line;
    std::vector<std::string> v;
    std::pair<std::string, std::string> header;
    size_t found;

    extractstr(line, requestMessage.buf, "\r\n\r\n");
    ft_split(v, line, "\r\n");
    for (std::vector<std::string>::iterator
            it = v.begin(); it != v.end(); it++) {
        found = it->find(":");
        if (found == std::string::npos)
            return ;//return error
        header = ft_slice_str(*it, found);
        addHeader(header);
    }
}

bool    HTTP::parseRequestBody() {
    int ret;

    if (requestMessage.non_body)
        requestMessage.buf = "";            //  non_body일 경우, 모든 body를 버리기

    if (requestMessage.body.size() > REQUEST_BODY_MAX_SIZE)
        setStatus(PATLOAD_TOO_LARGE);       //  body_len이 REQUEST_BODY_MAX_SIZE보다 크면 error

    if (requestMessage.content_length >= 0) //  Content-Length, Transfer-Encoding 모두 있을 경우
        ret = reqBodyContentLength();       //  Content-Length를 우선함
    else if (requestMessage.chunked)
        ret = reqBodyChunked();
    else {                                  //  Content-Length, Transfer-Encoding 모두 없을 경우
        requestMessage.non_body = true;
        ret = SUCCESS;
    }
    return ret;
}

/*
** request message - content-length를 받는 함수
*/
int HTTP::reqBodyContentLength() {
    size_t left_len = requestMessage.content_length - requestMessage.body.size();
    std::string temp;

    extractstr(temp, requestMessage.buf, left_len);
    requestMessage.body += temp;
    if (requestMessage.body.size() >= static_cast<size_t>(requestMessage.content_length))
        return SUCCESS_REQ;
    return FAIL_REQ;
}

int HTTP::reqBodyChunked() {
    std::string buf;
    for (; !requestMessage.buf.empty(); ) {
        if (requestMessage.chunk.getLength() < 0) {
            if (extractstr(buf, requestMessage.buf, "\r\n"))
                requestMessage.chunk.setLength(buf);
            else
                return FAIL_REQ;
            }
        else if (extractstr(buf, requestMessage.buf, "\r\n")) {
            requestMessage.chunk.appendContent(buf);
            requestMessage.chunk.setChunkEnd();
        }
        else
            return FAIL_REQ;

        if (requestMessage.chunk.isEndChunk()) {
            std::string const & s = requestMessage.chunk.getContent();
            if (s.size()) {
                requestMessage.body += s;
                requestMessage.chunk.initChunk();
            }
            else
                return SUCCESS_REQ;
        }
    }
    return FAIL_REQ;
}

void    HTTP::additionalParseRequestHeader() {
    if (requestMessage.header_in.find(CONTENT_LENGTH_STR) != requestMessage.header_in.end())
        requestMessage.content_length = std::strtod(requestMessage.header_in[CONTENT_LENGTH_STR].c_str(), 0);

    if (requestMessage.header_in[TRANSFER_ENCODING_STR] == "chunked")
        requestMessage.chunked = true;

    if (requestMessage.header_in.find(HOST_STR) != requestMessage.header_in.end()) {
        std::string host = requestMessage.header_in[HOST_STR];
        size_t      found = host.find(":");
        if (found != std::string::npos)
            requestMessage.port_num = static_cast<unsigned int>(std::strtod(ft_slice_str(host, found).second.c_str(), 0));
    }

    if (requestMessage.header_in.find(CONNECTION_STR) != requestMessage.header_in.end())
        if (requestMessage.header_in[CONNECTION_STR] == "close")
            requestMessage.keep_alive = false;

    requestMessage.method = methodStringtoBit(requestMessage.method_name);
    if (requestMessage.method == 0)
        setStatus(NOT_ALLOWED);
}

char    HTTP::methodStringtoBit(std::string str) {
    if (str == GET)
        return (GET_BIT);
    else if (str == PUT)
        return (PUT_BIT);
    else if (str == DELETE)
        return (DELETE_BIT);
    else if (str == POST)
        return (POST_BIT);
    else if (str == HEAD)
        return (HEAD_BIT);
    return 0;    
}

void    HTTP::reqInputBuf(std::string const & str) {
    requestMessage.buf += str;
    if (isReadyRequestLine()) {
        parseRequestLine();
        requestMessage.request_step = CLIENT_READ_REQ_HEADER;
    }
    if (isReadyRequestHeader()) {
        parseRequestHeader();
        additionalParseRequestHeader();
        requestMessage.request_step = CLIENT_READ_REQ_BODY;
    }
    if (isReadyRequestBody() &&
        parseRequestBody() == SUCCESS) {
        requestMessage.request_step = CLIENT_READ_FINISH;
    }
    if (requestMessage.request_step == CLIENT_READ_FINISH) {
        if (requestMessage.method_name == "POST" && requestMessage.body.empty())
            setStatus(NOT_ALLOWED);
        setStatus(0);
        // reqPrint();
    }
}

/*
** request message가 완료되었나 체크하는 함수
** true : 완료, false : 미완료
*/
bool HTTP::reqCheckFinished() {
	if (requestMessage.request_step == CLIENT_READ_FINISH)
		return true;
    return false;
}

bool HTTP::reqCheckHeaderFinished() {
    if (requestMessage.request_step == CLIENT_READ_REQ_BODY)
        return true;
    return false;
}

/*
** request message print
*/
void HTTP::reqPrint() {
    std::cout << "==============<<  request parsing finish  >>==============" << std::endl; 
    std::cout << requestMessage.method_name << " " << requestMessage.unparsed_uri << std::endl;
    for (std::map<std::string, std::string>::iterator it = requestMessage.header_in.begin(); it != requestMessage.header_in.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
    if (!requestMessage.non_body)
        std::cout << requestMessage.body << "<----------BODY END]" << std::endl;
    else
        std::cout << "************* there is no body ***************" << std::endl;
    std::cout << "port_num : " << requestMessage.port_num << std::endl
                << "keep-alive : " << requestMessage.keep_alive << std::endl
                << "content_length : " << requestMessage.content_length << std::endl
                << "chunked : " << requestMessage.chunked << std::endl
                << "method : " << static_cast<int>(requestMessage.method) << std::endl;
    std::cout << "==============<< ststus : " << status << " >>================" << std::endl;
}
