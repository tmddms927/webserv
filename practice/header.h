#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <syslog.h>

typedef	void (*sighandler_t)(int);
#define LINE_BUF_SIZE 4096
#define MAX_REQUEST_BODY_LENGTH (1024 * 1024)
#define BLOCK_BUF_SIZE 1024
#define TIME_BUF_SIZE 64
#define SERVER_NAME "seungoh's server"
#define SERVER_VERSION "1.0"
#define HTTP_MINOR_VERSION 1

#define USAGE "Usage: %s [--port=n] [--chroot --user=u --group=g] <docroot>\n"
#define MAX_BACKLOG 5
#define DEFAULT_PORT "90"


struct HTTPHeaderField {
	char *name;
	char *value;
	struct HTTPHeaderField *next;
};

struct HTTPRequest {
	// 요청에서 사용된 HTTP 버전
	int protocol_minor_version;
	// 요청 메소드(GET, HEAD, POST 등) 대문자로 지정
	char *method;
	// 요청 경로
	char *path;
	// HTTP 헤더
	struct HTTPHeaderField *header;
	// 엔티티 본문
	char *body;
	// 엔티티 본문 길이
	long length;
};

struct FileInfo {
	// 파일 시스템의 절대 경로
	char *path;
	// 파일 크기
	long size;
	// 0이면 파일이 존재안함
	int ok;
};

static struct option longopts[] = {
    {"debug",  no_argument,       &debug_mode, 1},
    {"chroot", no_argument,       NULL, 'c'},
    {"user",   required_argument, NULL, 'u'},
    {"group",  required_argument, NULL, 'g'},
    {"port",   required_argument, NULL, 'p'},
    {"help",   no_argument,       NULL, 'h'},
    {0, 0, 0, 0}
};

void log_exit(char *fmt, ...);
void* xmalloc(size_t sz);
void free_request(struct HTTPRequest *req);
char* build_fspath(char *docroot, char *urlpath);
struct FileInfo* get_fieldinfo(char *docroot, char *urlpath);
void free_fileinfo(struct FileInfo* info);
void trap_signal(int sig, sighandler_t handler);
void signal_exit(int sig);
void install_signal_handlers(void);
void upper(char *s);
void read_request_line(struct HTTPRequest *req, FILE *in);
struct HTTPHeaderField* read_header_field(FILE *in);
char* lookup_header_field_value(struct HTTPRequest *req, char *name);
long content_length(struct HTTPRequest *req);
struct HTTPRequest* read_request(FILE *in);
void respond_to(struct HTTPRequest *req, FILE *out, char *docroot);
void service(FILE *in, FILE *out, char *docroot);
void output_common_header_fields(struct HTTPRequest *req, FILE *out, char *status);
void do_file_response(struct HTTPRequest *req, FILE *out, char *docroot) ;


///////////////////////////
int listen_socket(char *port);
void server_main(int server_fd, char *docroot);
void detach_children(void);
void noop_handler(int sig);
