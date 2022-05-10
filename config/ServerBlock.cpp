//
// Created by gilee on 2022/05/05.
//

#include "ServerBlock.hpp"
#include <sstream>

typedef std::vector<std::string> rawtxt;

servers ServerBlock::parse(rawtxt &raw) {
    servers tmp;
    rawtxt::iterator it;

    tmp.port = 0;
    it = raw.begin();
    ++it;
    while (!(*it).empty() && (*it) != SERVERV) {
        if ((*it).find(LOCATIONV) != std::string::npos)
            tmp.location.push_back(parse_location(raw, it));
        if ((*it).find(PORT) != std::string::npos) {
            std::stringstream ss((*it).substr(strlen(PORT)));
            ss >> tmp.port;
        }
        if ((*it).find(HOSTV) != std::string::npos) {
            std::stringstream ss((*it).substr(strlen(HOSTV)));
            ss >> tmp.host;
        }
        it++;
        if (it == raw.end())
            break;
    }
    if (tmp.location.empty())
        throw InvalidServerBlock();
    /*
    if (!checkVariables(tmp, conf))
        throw InvalidServerBlock();
    if (tmp.port == 0)
        tmp.port = conf.port;
    */
    return tmp;
}

locations ServerBlock::parse_location(rawtxt & raw, rawtxt::iterator & it) {
    locations tmp;

    tmp.allowed_method = 0;
    tmp.is_aster = false;
    while (!(*it).empty()) {
        if ((*it).find(LOCATIONV) != std::string::npos) {
            std::stringstream ss((*it).substr(strlen(LOCATIONV)));
            ss >> tmp.location_uri;
            if (tmp.location_uri.find('*') != std::string::npos) {
                int found = tmp.location_uri.find('*');
                tmp.location_uri.replace(found, 1, "");
                tmp.is_aster = true;
            }
        }
        if ((*it).find(ROOT) != std::string::npos) {
            std::stringstream ss((*it).substr(strlen(ROOT)));
            ss >> tmp.location_root;
        }
        if ((*it).find(INDEXV) != std::string::npos) {
            std::stringstream ss((*it).substr(strlen(INDEXV)));
            ss >> tmp.index;
        }
        if ((*it).find(DEFAULT_ERROR) != std::string::npos) {
            std::stringstream ss((*it).substr(strlen(DEFAULT_ERROR)));
            ss >> tmp.err_page;
        }
        if ((*it).find(ALLOWED_METHODV) != std::string::npos)
            validMethod(tmp.allowed_method, (*it).substr(strlen(ALLOWED_METHODV)));
        it++;
    }
    raw.erase(raw.begin(), it);
    it = raw.begin();
    return tmp;
}

bool ServerBlock::checkVariables(const servers &tmp, const servers &src) {
    bool res = true;
    /*
    res |= (tmp.port != src.port)
        || (tmp.root == "")
        ;
        */
    return res;
}

const char *ServerBlock::InvalidServerBlock::what() const throw(){
    return "InvalidServerBlock";
}

const char *ServerBlock::InvalidLocationBlock::what() const throw() {
    return exception::what();
}

void ServerBlock::validMethod(char & allowed_bits, std::string const & methods) {
    std::string tmp = methods;
    if (tmp.find(CONF_GET) != std::string::npos) {
        allowed_bits |= GET_BIT;
        tmp.replace(tmp.find(CONF_GET), strlen(CONF_GET), "");
    }
    if (tmp.find(CONF_POST) != std::string::npos) {
        allowed_bits |= POST_BIT;
        tmp.replace(tmp.find(CONF_POST), strlen(CONF_POST), "");
    }
    if (tmp.find(CONF_DELETE) != std::string::npos) {
        allowed_bits |= DELETE_BIT;
        tmp.replace(tmp.find(CONF_DELETE), strlen(CONF_DELETE), "");
    }
    if (tmp.find(CONF_PUT) != std::string::npos) {
        allowed_bits |= PUT_BIT;
        tmp.replace(tmp.find(CONF_PUT), strlen(CONF_PUT), "");
    }
    if (tmp.find(CONF_HEAD) != std::string::npos) {
        allowed_bits |= HEAD_BIT;
        tmp.replace(tmp.find(CONF_HEAD), strlen(CONF_HEAD), "");
    }
    if (!tmp.empty())
        throw std::exception();

}

