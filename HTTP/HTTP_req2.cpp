#include "HTTP.hpp"
#include "../utils.hpp"

#include "../Server.hpp"
#include "HTTPHeaderField.hpp"
#include <vector>
#include <utility>

void    HTTP::addHeader(std::string line) {
    std::vector<std::string> v;
    ft_split(v, line, ": ");
    if (v.size() != 2)
        throw BAD_REQUEST;
    requestMessage.header_in.insert(std::pair<std::string, std::string>(v[0], v[1]));
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
    ft_split(v, line, " ");
    if (v.size() != 3)
        throw BAD_REQUEST;
    requestMessage.method = v[0];
    requestMessage.unparsed_uri = v[1];
    requestMessage.http_version = v[2];
}

void    HTTP::parseRequestHeader() {
    std::string line;
    std::vector<std::string> v;

    extractstr(line, requestMessage.buf, "\r\n\r\n");
    ft_split(v, line, "\r\n");
    for (std::vector<std::string>::iterator
            it = v.begin(); it != --v.end(); it++)
        addHeader(*it);
    if (requestMessage.header_in.find(CONTENT_LENGTH_STR) != requestMessage.header_in.end())
        requestMessage.content_length = std::strtod(requestMessage.header_in[CONTENT_LENGTH_STR].c_str(), 0);
    if (requestMessage.header_in[TRANSFER_ENCODING_STR] == "chunked")
        requestMessage.chunked = true;
}

bool    HTTP::parseRequestBody() {
    int ret;
    //  if non_body일 경우
    //      모든 body를 버리기
    //  body len 확인
    if (requestMessage.body.size() > REQUEST_BODY_MAX_SIZE)
        throw PATLOAD_TOO_LARGE;
    // Content-Length, Transfer-Encoding 모두 있을 경우, Content-Length를 우선함
    std::cout << "======" << requestMessage.content_length << " : " << requestMessage.chunked << "======" << std::endl;
    if (requestMessage.content_length >= 0)
        ret = reqBodyContentLength();
    else if (requestMessage.chunked)
        ret = reqBodyChunked();
    else { // Content-Length, Transfer-Encoding 모두 없을 경우
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

    requestMessage.body += requestMessage.buf.substr(0, left_len);
    if (requestMessage.body.size() >= requestMessage.content_length)
        return SUCCESS;
    return FAIL;
}

int HTTP::reqBodyChunked() {
    std::string buf;
    std::cout << "=================" << requestMessage.buf << std::endl;
    for (; !requestMessage.buf.empty(); ) {
        if (requestMessage.chunk.getLength() < 0) {
            if (extractstr(buf, requestMessage.buf, "\r\n"))
                requestMessage.chunk.setLength(buf);
            else
                return FAIL;
            }
        else if (extractstr(buf, requestMessage.buf, "\r\n")) {
            requestMessage.chunk.appendContent(buf);
            requestMessage.chunk.setChunkEnd();
        }
        else
            return FAIL;
        if (requestMessage.chunk.isEndChunk()) {
            std::string const & s = requestMessage.chunk.getContent();
            if (s.size())
                requestMessage.body += s;
            else
                return SUCCESS;
        }
    }
    return FAIL;
}


//set status

void    HTTP::reqInputBuf(std::string const & str) {
    //  if (isRequestEnd())
    //      str 버리기;
    //  else
    requestMessage.buf += str;
    std::cout << "======== buf start ==========" << std::endl;
    std::cout << requestMessage.buf << "==>buf end" << std::endl;
    try {
        //parse = read + store + validate check
        if (isReadyRequestLine()) {
            parseRequestLine();
            requestMessage.request_step = CLIENT_READ_REQ_HEADER;
            }
        if (isReadyRequestHeader()) {
            parseRequestHeader();
            requestMessage.request_step = CLIENT_READ_REQ_BODY;
            }
        if (isReadyRequestBody() &&
            parseRequestBody() == SUCCESS) {
            requestMessage.request_step = CLIENT_READ_FINISH;
            status = 0;
        }
    } catch (int & err) {
        requestMessage.buf = ""; //  buf 버리기
        requestMessage.request_step = CLIENT_READ_FINISH; //  request_step finish
        status = err; // 에러 발생 시 error 코드를 status에 할당

        std::cout << "  << CLIENT_READ_ERROR -->" << err  <<  "<-- >>  " << std::endl;
    }
}
