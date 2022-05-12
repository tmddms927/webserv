//
// Created by gilee on 2022/05/05.
//

#ifndef UNTITLED1_UTIL_H
#define UNTITLED1_UTIL_H
#include "constant.h"

typedef struct  global_s {
    int                    client_max_body_size;
}               global;

typedef struct location_s {
    std::string            location_uri;
    std::string            location_root;
    std::string            err_page;
    std::string            index;
    std::string            cgi;
    char                   allowed_method;
    bool                   is_aster;
}               locations;

typedef struct  servers_s {
    std::string            host;
    int                    port;
    std::vector<locations> location;
}               servers;

#endif //UNTITLED1_UTIL_H
