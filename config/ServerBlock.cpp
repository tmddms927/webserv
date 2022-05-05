//
// Created by gilee on 2022/05/05.
//

#include "ServerBlock.hpp"
#include <sstream>

typedef std::vector<std::string> rawtxt;
typedef std::vector<servers> conf;
servers ServerBlock::parse(rawtxt &raw, servers conf) {
    servers tmp;
    rawtxt::iterator it;

    tmp.port = 0;
    it = raw.begin();
    ++it;
    while (!(*it).empty()) {
        if ((*it).find(LOCATION) != std::string::npos) {
            tmp.location = (*it).substr(strlen(LOCATION));
            tmp.host = tmp.location;
        }
        if ((*it).find(PORT) != std::string::npos) {
            std::stringstream ss((*it).substr(strlen(PORT)));
            ss >> tmp.port;
        }
        if ((*it).find(CGI) != std::string::npos)
            tmp.cgi = (*it).substr(strlen(CGI));
        if ((*it).find(ROOT) != std::string::npos)
            tmp.root = (*it).substr(strlen(ROOT));
        it++;
    }
    if (tmp.port == 0)
        tmp.port = conf.port;
    return tmp;
}

const char *ServerBlock::InvalidServerBlock::what() const throw(){
    return "InvalidServerBlock";
}
