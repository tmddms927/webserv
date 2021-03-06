//
// Created by gilee on 2022/05/05.
//

#ifndef UNTITLED1_CONSTANT_H
#define UNTITLED1_CONSTANT_H

#define LOCATIONV "location "
#define SERVERV "server"
#define PORT "port "
#define HOSTV "host "
#define RETURNV "return "
#define ROOT "root "
#define CGI "cgi "
#define AUTO_INDEXV "auto_index "
#define CLIENT_BODY_SIZE "client_max_body_size "
#define DEFAULT_ERROR "default_error_page "
#define INDEXV "index "
#define ALLOWED_METHODV "allowed_method "

#define CONF_GET    "GET|"
#define CONF_POST   "POST|"
#define CONF_DELETE "DELETE|"
#define CONF_PUT    "PUT|"
#define CONF_HEAD   "HEAD|"

#define GET_BIT         1  //00000001
#define POST_BIT        2  //00000010
#define DELETE_BIT      4  //00000100
#define PUT_BIT         8  //00001000
#define HEAD_BIT        16 //00010000

#define RESPONSE_CODES  100, 200, 201, 204, 206, 301, 302, 303, 304, 307, 308, 401, 403, 404, 406, 407, 409, 410, 412, 416, 418, 425, 451, 500, 501, 502, 503, 504
#define RESPONSE_CNT    28
#endif //UNTITLED1_CONSTANT_H
