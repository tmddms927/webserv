//
// Created by Giyoung Lee on 5/12/22.
//

#ifndef UNTITLED1_AUTOINDEX_HPP
#define UNTITLED1_AUTOINDEX_HPP

#include "../HTTP/HTTP.hpp"
#include "../Server/Server.hpp"

class AutoIndex {
private:
    ResponseMessage                           _res;
    std::string                               _config;
    bool                                      _err;
public:
    explicit                            AutoIndex(std::string const & config);
    bool &                              makeHTML();
    const ResponseMessage               &getRes() const;
};


#endif //UNTITLED1_AUTOINDEX_HPP
