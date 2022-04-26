#include "RequestMessage.hpp"
#include <iostream>

RequestMessage::~RequestMessage() {}

RequestMessage::RequestMessage(uintptr_t fd) : socket_fd(fd), current(REQUEST_LINE) {}

bool RequestMessage::inputBuf(std::string const & str) {
    buf += str;
    if (bufCheck() == true)
        return parsing();
    return false;
}

std::string const & RequestMessage::getterBuf(void) const {
    return buf;
}

uintptr_t const & RequestMessage::getterSocketFd(void) const {
    return socket_fd;
}

bool RequestMessage::bufCheck() {
    if (buf.find("\\r\\n\\r\\n") == std::string::npos)
        return false;
    return true;
}

bool RequestMessage::parsing() {
    size_t index;
    std::string temp;

    while (1) {
        index = buf.find("\\r\\n");
        if (index == std::string::npos)
            break;
        temp = buf.substr(0, index);
        if (current == REQUEST_LINE)
            parsingRequestLine(temp);
        else if (current == HEADER_FIELD)
            parsingHeaderField(temp);
        else if (current == BODY) {
            if (parsingBody(temp))
                return true;
        }
        buf = buf.substr(index + 1);
    }
    return false;
}

void RequestMessage::parsingRequestLine(std::string & temp) {
    std::size_t index;

    // method
    index = temp.find(' ');
    if (index == std::string::npos)
        std::cout << "method error!" << index << '\n';
    method = temp.substr(0, index);
    temp = temp.substr(index + 1);

    // request-uri
    index = temp.find(' ');
    if (index == std::string::npos)
        std::cout << "request-uri error!" << index << '\n';
    path = temp.substr(0,index);
    temp = temp.substr(index + 1);

    // HTTP-version
    if (temp != "HTTP/1.1\\r\\n")
        std::cout << "HTTP-version error!" << index << '\n';
    protocol_minor_version = 1;
    current = HEADER_FIELD;
}

// error 나중에 제대로 하기!
void RequestMessage::parsingHeaderField(std::string const & temp) {
    size_t index;
    std::string first;
    std::string second;

    if (temp == "\\r\\n") {
        current = BODY;
        return ;
    }
    index = temp.find(":");
    if (!index)
        std::cout << "HeaderField [key:value] error. there are no key!" << std::endl;
    //todo !need to modify key and value validation check!
    first = temp.substr(0, index - 1);
    second = temp.substr(index + 1);
    header.insert(std::pair<std::string, std::string>(first, second));
}

bool RequestMessage::parsingBody(std::string const & temp) {
    if (temp == "\\r\\n") {
        return true;
    }
    body += temp;
    return false;
}
