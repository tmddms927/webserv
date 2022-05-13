//
// Created by gilee on 2022/05/05.
//

#include "ServerBlock.hpp"
#include <sstream>

#define FIND(X) (*it).find(X) != std::string::npos
#define GET_RAW_VALUE(X) std::stringstream ss((*it).substr(strlen(X)))
#define SET_TMP_VALUE(X) ss >> X
#define SERVER_BLOCK_END (*it).empty() && (*it) != SERVERV
#define UNDEFINED_END it->empty() && (it + 1) != raw.end() && *(it + 1) != SERVERV && (it + 1)->find(LOCATIONV) == std::string::npos
#define CHECK_NULL(X) if (X.empty()) throw InvalidLocationBlock()
typedef std::vector<std::string> rawtxt;

servers ServerBlock::parse(rawtxt &raw) {
    servers tmp;
    rawtxt::iterator it;

    tmp.port = 0;
    it = raw.begin();
    ++it;
    while (!SERVER_BLOCK_END) {
        if (FIND(LOCATIONV))
            tmp.location.push_back(parse_location(raw, it));
        if (FIND(PORT)) {
            GET_RAW_VALUE(PORT);
            SET_TMP_VALUE(tmp.port);
        }
        if (FIND(HOSTV)) {
            GET_RAW_VALUE(HOSTV);
            SET_TMP_VALUE(tmp.host);
        }
        it++;
        if (it == raw.end())
            break;
        if (it->empty() && (it + 1)->find(LOCATIONV) != std::string::npos)
            it++;
    }
    if (tmp.location.empty())
        throw InvalidServerBlock();
    return tmp;
}

locations ServerBlock::parse_location(rawtxt & raw, rawtxt::iterator & it) {
    locations tmp;

    tmp.allowed_method = 0;
    tmp.is_aster = false;
    tmp.auto_index = false;
    while (!(*it).empty()) {
        if (tmp.location_uri.empty() && FIND(LOCATIONV)) {
            GET_RAW_VALUE(LOCATIONV);
            SET_TMP_VALUE(tmp.location_uri);
            if (FIND("*")) {
                int found = tmp.location_uri.find('*');
                tmp.location_uri.replace(found, 1, "");
                tmp.is_aster = true;
            }
        }
        else if (tmp.location_root.empty() && FIND(ROOT)) {
            GET_RAW_VALUE(ROOT);
            SET_TMP_VALUE(tmp.location_root);
        }
        else if (tmp.index.empty() && FIND(INDEXV)) {
            GET_RAW_VALUE(INDEXV);
            SET_TMP_VALUE(tmp.index);
        }
        else if (tmp.err_page.empty() && FIND(DEFAULT_ERROR)) {
            GET_RAW_VALUE(DEFAULT_ERROR);
            SET_TMP_VALUE(tmp.err_page);
        }
        else if (tmp.allowed_method == 0 && FIND(ALLOWED_METHODV))
            validMethod(tmp.allowed_method, (*it).substr(strlen(ALLOWED_METHODV)));
        else if (tmp.cgi.empty() && FIND(CGI)) {
            GET_RAW_VALUE(CGI);
            SET_TMP_VALUE(tmp.cgi);
            CHECK_NULL(tmp.cgi);
        }
        else if (tmp.auto_index == 0 && FIND(AUTO_INDEXV)) {
            std::string value;
            GET_RAW_VALUE(AUTO_INDEXV);
            ss >> value;
            if (value == "on")
                tmp.auto_index = true;
            else if (value == "off")
                tmp.auto_index = false;
            else
                throw InvalidLocationBlock();
        }
        else
            throw InvalidLocationBlock();
        it++;
        if (UNDEFINED_END)
            throw InvalidLocationBlock();
    }
    raw.erase(raw.begin(), it);
    it = raw.begin();
    return tmp;
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

const char *ServerBlock::InvalidServerBlock::what() const throw(){
    return "InvalidServerBlock";
}

const char *ServerBlock::InvalidLocationBlock::what() const throw() {
    return "InvalidLocationBlock";
}
