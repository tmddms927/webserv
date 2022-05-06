#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"
#include "Server.hpp"

#include <vector>
#include <map>
#include <utility>

void    ft_split(std::vector<std::string> &dest, std::string const &src, std::string const &mark);

/*
** request message가 왔을 경우 request.buf에 메시지 저장
*/
void HTTP::reqInputBuf(std::string const & str) {
    requestMessage.buf += str;

    try {
        if (requestMessage.current == CLIENT_READ_REQ_LINE)
            if (process_request_line())
                requestMessage.current = CLIENT_READ_REQ_HEADER;
        if (requestMessage.current == CLIENT_READ_REQ_HEADER)
            if (process_request_headers())
                requestMessage.current = CLIENT_READ_REQ_BODY;
        if (requestMessage.current == CLIENT_READ_REQ_BODY)
            if (process_request_body())
                requestMessage.current = CLIENT_READ_FINISH;
        if (requestMessage.current == CLIENT_READ_FINISH) {
            // std::cout << "method : [" << requestMessage.method << "]" << std::endl   //  apply config <- rewrite uri
            //             << "path : [" << requestMessage.path << "]" << std::endl
            //             << "content-length : ["<< requestMessage.header_in[CONTENT_LENGTH_STR] << "]" << std::endl;

            // std::cout << std::endl << "<< CLIENT_READ_FINISH >> " << std::endl;
        }

                //  method setting <- if POST && NON_BODY => error! || if GET && uri not fie => error!
                //  event resist
    } catch (int) {

    }
}

/*
** request message - request line를 파싱하는 함수
*/
int HTTP::process_request_line() {
    std::size_t index;

    // std::cout << "=====================\n" <<  requestMessage.buf << std::endl << "=====================\n";

    size_t found;
    if ((found = requestMessage.buf.find("\r\n")) == std::string::npos)
        return 0;

    std::string temp = requestMessage.buf.substr(0, found);
    requestMessage.buf = requestMessage.buf.substr(found + 2);

    // method
    index = temp.find(' ');
    if (index == std::string::npos)
        std::cout << "method error!" << index << '\n';
    requestMessage.method = temp.substr(0, index);
    temp = temp.substr(index + 1);

    // request-uri
    index = temp.find(' ');
    if (index == std::string::npos)
        std::cout << "request-uri error!" << index << '\n';
    requestMessage.path = temp.substr(0,index);
    temp = temp.substr(index + 1);

    // HTTP-version
    if (temp != "HTTP/1.1" && temp != "HTTP/1.0")
        std::cout << "HTTP-version error!" << index << '\n';
    protocol_minor_version = 1;
    // requestMessage.current = REQ_HEADER_FIELD;
    return 1;
}

/* 
** request header를 파싱하는 함수
*/
int        HTTP::process_request_headers() {
    std::vector<std::string> req_header_field_splited;

    size_t found;
    if ((found = requestMessage.buf.find("\r\n\r\n")) == std::string::npos)
        return 0;

    std::string temp = requestMessage.buf.substr(0, found);
    requestMessage.buf = requestMessage.buf.substr(found + 4);


    ft_split(req_header_field_splited, temp, "\r\n");
    for (size_t i = 0; i < req_header_field_splited.size(); i++) {
        std::vector<std::string> node;
        ft_split(node , req_header_field_splited[i], ":");
        if (node.size() != 2)
            continue;
        //if error occur -> return 400(bad reqeust)
        requestMessage.header_in.insert(std::pair<std::string, std::string>(node[0], node[1]));
    }
    return 1;
}

/* 
** request body를 파싱하는 함수
*/
int     HTTP::process_request_body() {
    requestMessage.content_length = std::strtod(requestMessage.header_in["Content-Length"].c_str(), 0);

    if (requestMessage.content_length >= 0) {
        if (reqBodyContentLength())
            return 1;
    }
    else if (requestMessage.content_length == -1) {
        if (reqBodyChunked())
            return 1; // reqBodyChunked 함수 안에서 chunk가 완성되지 않은 채 들어올 경우도 고려해야함
    }
    // if (requestMessage.current == CLIENT_READ_FINISH) {
    //     std::cout << "REQ_FINISHED" << std::endl;
    //     return 0;
    // }
    std::cout << "REQ_NOT_FINISHED" << std::endl;
    return 0;
}

/*
** request message - content-length를 받는 함수
*/
int HTTP::reqBodyContentLength() {
    size_t len = std::strtod(requestMessage.header_in["Content-Length"].c_str(), 0);

    requestMessage.body += requestMessage.buf.substr(0, len - requestMessage.body.size());
    if (requestMessage.body.size() >= len)
        return 1;
    return 0;
}

/*
** request message - chunked body를 받는 함수
*/
int HTTP::reqBodyChunked() {
    std::stringstream stream;

    if (requestMessage.chunk.length == 0) {
        if (requestMessage.buf == "") {
            requestMessage.current = CLIENT_READ_FINISH;
            return 1;
        }
    }
    // if (requestMessage.chunk.length == -1) {
    //     stream << temp;
    //     stream >> std::hex >> requestMessage.chunk.length;
    //     std::cout << "requestMessage.chunk.length ! : " << requestMessage.chunk.length << std::endl;
    // }
    // else {
    //     requestMessage.chunk.content += temp;
    //     requestMessage.chunk.length -= temp.length();
    //     if (requestMessage.chunk.length <= 0)
    //         reqChunkInit();
    // }
    return 0;
}

/*
** request message가 완료되었나 체크하는 함수
** true : 완료, false : 미완료
*/
bool HTTP::reqCheckFinished() {
	if (requestMessage.current == CLIENT_READ_FINISH)
		return true;
	else
		return false;
}

/*
** request message print
*/
void HTTP::reqPrint() {
    std::cout << "message : " << requestMessage.method << std::endl;
    std::cout << "path : " << requestMessage.path << std::endl;
    for (std::map<std::string, std::string>::iterator it = requestMessage.header_in.begin(); it != requestMessage.header_in.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
    std::cout << "body : " << requestMessage.body << std::endl;
}

void    ft_split(std::vector<std::string> &dest, std::string const &src, std::string const &mark) {
    size_t start_pos = 0;

    for (int debug_count = 0;;debug_count++) {
        size_t found;

        if (start_pos >= src.size()) {
            break;
        }
        found = src.find(mark, start_pos);
        if (found == std::string::npos) {
            std::string s = src.substr(start_pos);
            dest.push_back(s);
            break;
        }
        dest.push_back(src.substr(start_pos, found - start_pos));
        start_pos = found + mark.size();
    }
}
