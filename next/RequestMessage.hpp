#ifndef REQUESTMESSAGE_HPP
#define REQUESTMESSAGE_HPP

#include "HeaderFieldInfo.hpp"
#include <map>
#include <iostream>
//#include "next/ResponseMessage.hpp"

//todo binary에대한 처리 생각하기!

typedef std::map<std::string, std::string> HTTPHeaderField;

#define REQUEST_LINE        0
#define HEADER_FIELD        1
#define BODY                2

class RequestMessage{
private:
    std::string             method;
    std::string             path;
    HTTPHeaderField         header;
    std::string             body;
    long                    length;
    std::string             buf;
    int                     err_num;
    int                     current;
public:
    RequestMessage(uintptr_t fd);
    virtual ~RequestMessage();
    bool inputBuf(std::string const & str);
    std::string const & getterBuf(void) const;
    uintptr_t const & getterSocketFd(void) const;
    bool bufCheck();
    bool parsing();
    void parsingRequestLine(std::string & temp);
    void parsingHeaderField(std::string const & temp);
    bool parsingBody(std::string const & temp);
};

#endif
