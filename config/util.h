//
// Created by gilee on 2022/05/05.
//

#ifndef UNTITLED1_UTIL_H
#define UNTITLED1_UTIL_H
#include "constant.h"

typedef struct  global_s {
    std::string err_page;
    int         client_max_body_size;
    std::string index;
}               global;

typedef struct  servers_s {
    std::string host;
    std::string location;
    std::string cgi;
    std::string root;
    int         port;
}               servers;
#endif //UNTITLED1_UTIL_H
