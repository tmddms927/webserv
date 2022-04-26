#ifndef REQUESTMESSAGE_HPP
#define REQUESTMESSAGE_HPP
#include "HeaderFieldInfo.hpp"
#include <map>

typedef std::map<std::string, std::string> HTTPHeaderField;
//todo binary에대한 처리 생각하기!
class RequestMessage{
private:
    int                     protocol_minor_version;
    std::string             method;
    std::string             path;
    HTTPHeaderField         header;
    std::string             body;
    long                    length;
    std::string             buf;
public:
    RequestMessage();
    virtual ~RequestMessage();
    RequestMessage(std::string const & str);
    void inputBuf(std::string const & str);
    std::string getterBuf(void);
    bool bufCheck();
};

#endif
