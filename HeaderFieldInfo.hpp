//
// Created by gilee on 2022/04/20.
//

#ifndef WEBSERV_HEADERFIELDINFO_HPP
#define WEBSERV_HEADERFIELDINFO_HPP

#include <vector>
#include <string>

class HeaderFieldInfo {
private:
    std::vector<const std::string>    key;
public:
    HeaderFieldInfo();
    const std::string&  operator[](unsigned int idx) const;
};


#endif //WEBSERV_HEADERFIELDINFO_HPP
