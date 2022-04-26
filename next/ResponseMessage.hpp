#ifndef RESPONSEMESSAGE_HPP
#define RESPONSEMESSAGE_HPP

#include "../RequestMessage.hpp"
#include <iostream>
#include <unistd.h>
class RequestMessage;
class ResponseMessage {
private:
    const RequestMessage requestMessage;
    std::string header;
    std::string body;
public:
    ResponseMessage(RequestMessage const & rm);

    void sendMessage();
};

#endif
