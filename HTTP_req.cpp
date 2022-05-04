#include "HTTP.hpp"
#include "HTTPHeaderField.hpp"

#include <vector>
#include <map>
#include <utility>

void    ft_split(std::vector<std::string> &dest, std::string const &src, std::string const &mark);

/*
** request message가 왔을 경우 request.buf에 메시지 저장
*/
void HTTP::reqInputBuf(std::string const & str) {
    std::cout << "istream occur!! [" << str << "]" << std::endl;
    requestMessage.buf += str;

    if (requestMessage.current == PRE_READ_REQUEST_HEADER && reqbufCheck() == true) {
        try {
            reqParsing(); //parse request_line, request_header
            //apply server block, location block configure
            requestMessage.current = POST_READ_REQUEST_HEADER;
        } catch (int & e){
            // finished request because of error
        }
    }
    if (requestMessage.current == POST_READ_REQUEST_HEADER) {
        try {
            if (process_request_body(str)) { // process body(content-length, chunked)
                content_phase();            // generate response
                std::cout << "method : " << requestMessage.method << std::endl
                          << "uri : " << requestMessage.path << std::endl
                          << "Content-Length : " << requestMessage.header_in[CONTENT_LENGTH_STR] << std::endl
                          << "body : " << requestMessage.body << std::endl;
                //finish request normally
            }
        } catch (int & e) {
            // finished request because of error
        }
    }
}

/*
** request message에 "\r\n"이 있는지 체크
*/
bool HTTP::reqbufCheck() {
    if (requestMessage.buf.find("\r\n\r\n") == std::string::npos)
        return false;
    return true;
}

void HTTP::reqParsing() {
    size_t found = requestMessage.buf.find("\r\n");

    std::string s = requestMessage.buf.substr(0, found);
    reqParsingRequestLine(s); //process_request_line()

    requestMessage.buf = requestMessage.buf.substr(found + 2);
    found = requestMessage.buf.find("\r\n\r\n");
    process_request_headers(requestMessage.buf.substr(0, found));

    if (requestMessage.buf.size() > found + 4)
        requestMessage.buf = requestMessage.buf.substr(found + 4);
    else
        requestMessage.buf = "";
}

// check syntax error and value validation each line
int        HTTP::process_request_headers(std::string const &req_header_field) {
    std::vector<std::string> req_header_field_splited;

    ft_split(req_header_field_splited, req_header_field, "\r\n");
    for (size_t i = 0; i < req_header_field_splited.size(); i++) {
        std::vector<std::string> node;
        ft_split(node , req_header_field_splited[i], ":");
        if (node.size() != 2)
            continue;
        //if error occur -> return 400(bad reqeust)
        requestMessage.header_in.insert(std::pair<std::string, std::string>(node[0], node[1]));
    }
    return 200;
}

int     HTTP::process_request_body(std::string const & part_of_body) {
    if (requestMessage.content_length >= 0)
        reqBodyContentLength(part_of_body);
    else if (requestMessage.content_length == -1)
        reqBodyChunked(part_of_body); // reqBodyChunked 함수 안에서 chunk가 완성되지 않은 채 들어올 경우도 고려해야함
    if (requestMessage.current == REQ_FINISHED) {
        std::cout << "REQ_FINISHED" << std::endl;
        return 1;
    }
    std::cout << "REQ_NOT_FINISHED" << std::endl;
    return 0;
}

void     HTTP::content_phase() {
    if (requestMessage.method == "GET")
       reqGETHeaderCheck();
    if (requestMessage.method == "POST")
       reqPOSTHeaderCheck();
}











//todo 연결 끊어졌을때 HTTP객체도 제대로 없어져야해!!!!! 기억해!!!!! 꼭 고쳐!!!! 미래의 승은님
//void    HTTP::reqMethodPostProcess() {
//
//}

/*
** request message - request line를 파싱하는 함수
*/
void HTTP::reqParsingRequestLine(std::string & temp) {
    std::size_t index;

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
}

/*
 *  header field에서 body를 어떻게 받는지(chuncked인지, content-length인지)를 확인
 */
// todo invalid header 이전 parsingHeader에서 체크
void HTTP::bodyEncodingType(){
    if (requestMessage.header_in.find(CONTENT_LENGTH_STR) !=  requestMessage.header_in.end()
        && requestMessage.header_in.find(TRANSFER_ENCODING_STR) != requestMessage.header_in.end())
        throw BAD_REQUEST;
    if (requestMessage.header_in.find(CONTENT_LENGTH_STR) ==  requestMessage.header_in.end()
        && requestMessage.header_in.find(TRANSFER_ENCODING_STR) == requestMessage.header_in.end())
        throw NO_BODY;
    if (requestMessage.header_in.find(CONTENT_LENGTH_STR) !=  requestMessage.header_in.end())
        throw REQ_CONTENT_LENGTH;
    // todo transfer-encoding value가 chunked가 아닌 경우도 고려
    if (requestMessage.header_in.find(TRANSFER_ENCODING_STR) != requestMessage.header_in.end()) {
        reqChunkInit();
        throw REQ_CHUNKED;
    }
}

/*
** request message - chunked body를 받는 함수
*/
void HTTP::reqBodyChunked(std::string const & temp) {
    std::stringstream stream;

    if (requestMessage.chunk.length == 0) {
        if (temp == "")
            requestMessage.current = REQ_FINISHED;
    }
    if (requestMessage.chunk.length == -1) {
        stream << temp;
        stream >> std::hex >> requestMessage.chunk.length;
        std::cout << "requestMessage.chunk.length ! : " << requestMessage.chunk.length << std::endl;
    }
    else {
        requestMessage.chunk.content += temp;
        requestMessage.chunk.length -= temp.length();
        if (requestMessage.chunk.length <= 0)
            reqChunkInit();
    }
}

/*
** get GET header Check
*/
void HTTP::reqGETHeaderCheck() {
    try {
        bodyEncodingType();
    }
    catch (int const & status) {
        if (status == BAD_REQUEST) //content-length, transfer-encoding both true
            std::cout << "reqGETHeaderCheck : header error! (bad request)" << std::endl;
        else if (status == NO_BODY) //content-length, transfer-encoding both false
            requestMessage.current = REQ_FINISHED;
        else
            requestMessage.current = status;
    }
}

/*
** get POST Header Check
*/
void HTTP::reqPOSTHeaderCheck() {
    try {
        bodyEncodingType();
    }
    catch (int const & status) {
        if (status == BAD_REQUEST) //content-length, transfer-encoding both true
            std::cout << "reqGETHeaderCheck : header error! (bad request)" << std::endl;
        else if (status == NO_BODY) //content-length, transfer-encoding both false
            requestMessage.current = BAD_REQUEST; //todo error throw
        else
            requestMessage.current = status;
    }
}

/*
** request message - content-length를 받는 함수
*/
void HTTP::reqBodyContentLength(std::string const & temp) {
    (void)temp;
    
    requestMessage.body += requestMessage.buf;
    if (requestMessage.body.length() >= std::strtod(requestMessage.header_in["Content-Length"].c_str(), 0))
        requestMessage.current = REQ_FINISHED;
}

/*
** request message가 완료되었나 체크하는 함수
** true : 완료, false : 미완료
*/
bool HTTP::reqCheckFinished() {
	if (requestMessage.current == REQ_FINISHED)
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
    if (requestMessage.chunk.content.length() != 0)
        requestMessage.body += requestMessage.chunk.content;
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




