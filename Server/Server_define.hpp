#ifndef Server_DEFINE_HPP
#define Server_DEFINE_HPP

#define SOCKET_LISTEN_BACKLOG   5
#define SOCKET_PORT             80
#define SOCKET_ADDR             INADDR_ANY
#define SOCKET_READ_BUF         16384
#define KQUEUE_EVENT_LIST_SIZE  1024
#define REQUEST_BODY_MAX_SIZE   100000001
#define RECIEVE_BODY_MAX_SIZE	16384
#define RW_MAX_SIZE				16384
#define SERVER_DEFAULT_NAME		"webserv"
#define TIME_OUT                600

#define CGI_FINISHED            1001
#define CGI_ERROR               1002
#define CGI_NOT_FINISHED		1003

#endif
