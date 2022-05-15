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
    int                    redirect_code;
    std::string            redirect_uri;
    int                    client_max_body_size;
    char                   allowed_method;
    int                    is_aster;
    int                    auto_index;
}               locations;

typedef struct  servers_s {
    std::string            host;
    int                    port;
    std::vector<locations> location;
}               servers;

#endif //UNTITLED1_UTIL_H
