#include "RequestMessage.hpp"


RequestMessage::RequestMessage(std::string const & str): buf(str) {}

void RequestMessage::inputBuf(std::string const & str) {
    buf += str;
    if (bufCheck() == true)
        //go parsign
}

RequestMessage::~RequestMessage() {}

RequestMessage::RequestMessage() {}

std::string RequestMessage::getterBuf(void) {
    return buf;
}

bool RequestMessage::bufCheck() {
    if (buf.find("\\r\\n\\r\\n") == std::string::npos)
        return false;
    return true;
}
