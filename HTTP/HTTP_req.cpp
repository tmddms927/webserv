#include "HTTP.hpp"
#include "../utils.hpp"

#include "../Server/Server.hpp"
#include "HTTPHeaderField.hpp"
#include <vector>
#include <utility>

void    HTTP::addHeader(std::string line) {
    std::vector<std::string> v;
    ft_split(v, line, ":");
    if (v.size() != 2)
        throw BAD_REQUEST;
    ft_trim_space(v[0]);
    ft_trim_space(v[1]);
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
            it = v.begin(); it != v.end(); it++)
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
    std::string temp;

    extractstr(temp, requestMessage.buf, left_len);
    requestMessage.body += temp;
    if (requestMessage.body.size() >= static_cast<size_t>(requestMessage.content_length))
        return SUCCESS;
    return FAIL;
}

int HTTP::reqBodyChunked() {
    std::string buf;
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
            if (s.size()) {
                requestMessage.body += s;
                requestMessage.chunk.initChunk();
            }
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
            if (requestMessage.method == "POST" && requestMessage.body.empty())
                throw NOT_ALLOWED;
            reqPrint();
        }
    } catch (int & err) {
        requestMessage.buf = ""; //  buf 버리기
        requestMessage.request_step = CLIENT_READ_FINISH; //  request_step finish
        status = err; // 에러 발생 시 error 코드를 status에 할당

        std::cout << "  << CLIENT_READ_ERROR -->" << err  <<  "<-- >>  " << std::endl;
    }
}

/*
** request message가 완료되었나 체크하는 함수
** true : 완료, false : 미완료
*/
bool HTTP::reqCheckFinished() {
	if (requestMessage.request_step == CLIENT_READ_FINISH)
		return true;
	else
		return false;
}

/*
** request message print
*/
void HTTP::reqPrint() {
    std::cout << "==============<<  request parsing finish  >>==============" << std::endl; 
    std::cout << requestMessage.method << " " << requestMessage.unparsed_uri << std::endl;
    for (std::map<std::string, std::string>::iterator it = requestMessage.header_in.begin(); it != requestMessage.header_in.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
    if (!requestMessage.non_body)
                std::cout << requestMessage.body << "<----------BODY END]" << std::endl;
            else
                std::cout << "************* there is no body ***************" << std::endl;
}
