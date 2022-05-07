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

    try { // 이 try문의 함수들은 에러 발생시 무조건 throw로 적절한 error 코드(400 Bad Request, 405 Not Allow 등)를 반환하여야 한다.
        if (requestMessage.request_step == CLIENT_READ_REQ_LINE &&
                process_request_line() == SUCCESS) {
            //isAllowedMethod()
            if (requestMessage.method != GET && requestMessage.method != POST && requestMessage.method != DELETE &&
                requestMessage.method != HEAD)
                throw NOT_ALLOWED;
            requestMessage.request_step = CLIENT_READ_REQ_HEADER;
        }
        if (requestMessage.request_step == CLIENT_READ_REQ_HEADER &&
                process_request_headers() == SUCCESS) {

            HTTPHeaderField::iterator it_content_len = requestMessage.header_in.find(CONTENT_LENGTH_STR);
            if (it_content_len != requestMessage.header_in.end())
                requestMessage.content_length =  std::strtod(it_content_len->second.c_str(), 0);

            HTTPHeaderField::iterator it_transfer_enco = requestMessage.header_in.find(TRANSFER_ENCODING_STR);
            if (it_transfer_enco != requestMessage.header_in.end() && it_transfer_enco->second == "chunked")
                requestMessage.chunked = true;

            requestMessage.request_step = CLIENT_READ_REQ_BODY;
        }
        if (requestMessage.request_step == CLIENT_READ_REQ_BODY &&
                process_request_body() == SUCCESS)
            requestMessage.request_step = CLIENT_READ_FINISH;
        if (requestMessage.request_step == CLIENT_READ_FINISH)
            status = 0;
    } catch (int & err) {
        // 에러 발생 시 error 코드를 status에 할당
        requestMessage.request_step = CLIENT_READ_FINISH;
        std::cout << "  << CLIENT_READ_ERROR -->" << err  <<  "<-- >>  " << std::endl;
        status = err;
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
        return FAIL;

    std::string temp = requestMessage.buf.substr(0, found);
    requestMessage.buf = requestMessage.buf.substr(found + 2);

    std::vector<std::string> request_line_buf;
    ft_split(request_line_buf, temp, " ");

    if (request_line_buf.size() != 3)
        throw NOT_ALLOWED;

    // HTTP-version
    requestMessage.http_version = request_line_buf.back();
    request_line_buf.pop_back();
    // request-uri
    requestMessage.unparsed_uri = request_line_buf.back();
    request_line_buf.pop_back();
    // method
    requestMessage.method = request_line_buf.back();
    request_line_buf.pop_back();
    return SUCCESS;
}

/* 
** request header를 파싱하는 함수
*/
int        HTTP::process_request_headers() {
    std::vector<std::string> req_header_field_splited;

    size_t found;
    if ((found = requestMessage.buf.find("\r\n\r\n")) == std::string::npos)
        return FAIL;

    std::string temp = requestMessage.buf.substr(0, found);
    requestMessage.buf = requestMessage.buf.substr(found + 4);

    ft_split(req_header_field_splited, temp, "\r\n");
    for (size_t i = 0; i < req_header_field_splited.size(); i++) {
        std::vector<std::string> node;
        ft_split(node , req_header_field_splited[i], ": ");
        if (node.size() != 2)
            throw BAD_REQUEST;
        //isAllowdeHeader() -> if error occur -> return 400(bad reqeust)
        requestMessage.header_in.insert(std::pair<std::string, std::string>(node[0], node[1]));
    }
    return SUCCESS;
}

/* 
** request body를 파싱하는 함수
*/
int     HTTP::process_request_body() {
    if (requestMessage.content_length == -1 && !requestMessage.chunked) {
        requestMessage.non_body = true;
        return SUCCESS;
    }
    if (requestMessage.content_length >= 0) {
    std::cout << "=========x=====process_request_body============" << std::endl;
        if (reqBodyContentLength() == SUCCESS)
            return SUCCESS;
    }
    // else if (requestMessage.chunked) {
    else {
        if (reqBodyChunked() == SUCCESS)
            return SUCCESS; // reqBodyChunked 함수 안에서 chunk가 완성되지 않은 채 들어올 경우도 고려해야함
    }
    return FAIL;
    //만약 content-length, chunked 둘다 true라면??
}

/*
** request message - content-length를 받는 함수
*/
int HTTP::reqBodyContentLength() {
    if (requestMessage.content_length > RECIEVE_BODY_MAX_SIZE) //content_length가 기준인지 읽어진 body 사이즈 기준인지 애매함
        throw PATLOAD_TOO_LARGE;
    requestMessage.body += requestMessage.buf.substr(0, requestMessage.content_length - requestMessage.body.size());
    std::cout << "requestMessage.body : " << requestMessage.body << std::endl;
    if (requestMessage.body.size() >= requestMessage.content_length)
        return SUCCESS;
    return FAIL;
}

/*
** request message - chunked body를 받는 함수
*/
int HTTP::reqBodyChunked() {
    std::stringstream stream;

    if (requestMessage.chunk.length == 0) {
        size_t found = requestMessage.buf.find("\r\n");
        if (found == std::string::npos)
            return FAIL;
        std::string temp = requestMessage.buf.substr(0, found);
        if (temp == "") {
            return SUCCESS;
        }
    }
    if (requestMessage.chunk.length == -1) {
        size_t found = requestMessage.buf.find("\r\n");
        if (found == std::string::npos)
            return FAIL;
        std::string temp = requestMessage.buf.substr(0, found);
        requestMessage.buf = requestMessage.buf.substr(found + 2);

        stream << temp;
        stream >> std::hex >> requestMessage.chunk.length;
    }
    else {
        std::string temp = requestMessage.buf.substr(0, requestMessage.chunk.length);
        requestMessage.chunk.content += temp;
        requestMessage.chunk.length -= temp.length();
        if (requestMessage.chunk.length <= 0) {
            requestMessage.body += requestMessage.chunk.content;
            if (requestMessage.body.size() > RECIEVE_BODY_MAX_SIZE)
                throw PATLOAD_TOO_LARGE;
            reqChunkInit();
        }
    }
    return FAIL;
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
    std::cout << "message : " << requestMessage.method << std::endl;
    std::cout << "path : " << requestMessage.path << std::endl;
    for (std::map<std::string, std::string>::iterator it = requestMessage.header_in.begin(); it != requestMessage.header_in.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
    std::cout << "body : " << requestMessage.body << std::endl;
}

/*
 * chunk struct 초기화 하는 함수
 * length = -1, content = ""
 */
void HTTP::reqChunkInit() {
    requestMessage.chunk.length = -1;
    requestMessage.chunk.content = "";
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
