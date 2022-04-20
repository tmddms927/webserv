#include "header.h"

static int debug_mode = 0;

// 에러 메세지 출력 후 프로그램 종료
void log_exit(char *fmt, ...) {
	va_list ap;

	va_start(ap,fmt);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
	va_end(ap);
	exit(1);
}

// 사이즈를 받아서 말록해주는 함수
void* xmalloc(size_t sz) {
	void *p;

	p = malloc(sz);
	if (!p)
		log_exit("failed to allocate memory");
	return (p);
}

// HTTPRequest 사용 후 free 해주는 함수
void free_request(struct HTTPRequest *req) {
	struct HTTPHeaderField *h, *head;

	head = req->header;
	while (head) {
		h = head;
		head = head->next;
		free(h->name);
		free(h->value);
		free(h);
	}
	free(req->method);
	free(req->path);
	free(req->body);
	free(req);
}

// url 요청에 대한 파일 시스템 경로 생성
char* build_fspath(char *docroot, char *urlpath) {
	char *path;

	path = xmalloc(strlen(docroot) + 1 + strlen(urlpath) + 1);
	sprintf(path, "%s/%s", docroot, urlpath);
	return path;
}

// FileInfo 메모리 할당 & 값을 지정
struct FileInfo* get_fieldinfo(char *docroot, char *urlpath) {
	struct FileInfo *info;
	struct stat st;

	info = xmalloc(sizeof(struct FileInfo));
	// URL 요청에 대한 파일 시스템 경로 생성
	info->path = build_fspath(docroot, urlpath);
	info->ok = 0;
	// 1. 엔트리 존재 2. 보통 파일인지 확인
	if (lstat(info->path, &st) < 0)
		return info;
	if (!S_ISREG(st.st_mode))
		return info;
	info->ok = 1;
	info->size = st.st_size;
	return info;
}

// FileInfo 메모리 해제
void free_fileinfo(struct FileInfo* info) {
	free(info->path);
	free(info);
}

void trap_signal(int sig, sighandler_t handler) {
	struct sigaction act;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if (sigaction(sig, &act, NULL) < 0)
		log_exit("sigaction() failed: %s", strerror(errno));
}

// signal 번호로 프로그램 종료
void signal_exit(int sig) {
	log_exit("exit by signal %d", sig);
}

// 시그널 핸들러 추가
void install_signal_handlers(void) {
	trap_signal(SIGPIPE, signal_exit);;
}

// 대문자로 변환
void upper(char *s) {
	int i = 0;

	while (s[i]) {
		if ('a' <= s[i] && s[i] <= 'z')
			s[i] -= 32;
		i++;
	}
}

void read_request_line(struct HTTPRequest *req, FILE *in) {
	char buf[LINE_BUF_SIZE];
	char *path, *p;

	if (!fgets(buf, LINE_BUF_SIZE, in))
		log_exit("no request line");
	p = strchr(buf, ' ');
	if (!p)
		log_exit("parse error on request line (1): %s", buf);

	*p++ = 0;
	req->method = xmalloc(p - buf);
	strcpy(req->method, buf);
	upper(req->method);

	path = p;
	p = strchr(path, ' ');
	if (!p)
		log_exit("parse error on request line (2): %s", buf);
	*p++ = 0;
	req->path = xmalloc(p - path);
	strcpy(req->path, path);

	if (strncasecmp(p, "HTTP/1.", strlen("HTTP/1.")) != 0)
		log_exit("parse error on request line (3): %s", buf);
	p += strlen("HTTP/1.");
	req->protocol_minor_version = atoi(p);
}

struct HTTPHeaderField* read_header_field(FILE *in) {
	struct HTTPHeaderField *h;
	char buf[LINE_BUF_SIZE];
	char *p;

	if (!fgets(buf, LINE_BUF_SIZE, in)) {
		log_exit("failed to read request header field: %s", strerror(errno));
	}
	if ((buf[0] == '\n') || (strcmp(buf, "\r\n") == 0))
		return NULL;
	p = strchr(buf, ':');
	if (!p)
		log_exit("parse error on request header field: %s", buf);
	*p++ = 0;

	h = xmalloc(sizeof(struct HTTPHeaderField));
	h->name = xmalloc(p - buf);
	strcpy(h->name, buf);

	p += strspn(p, "\t");
	h->value = xmalloc(strlen(p) + 1);
	strcpy(h->value, p);
	return h;
}

char* lookup_header_field_value(struct HTTPRequest *req, char *name) {
	struct HTTPHeaderField *h;

	for (h = req->header; h; h = h->next) {
		if (strcasecmp(h->name, name) == 0)
			return h->value;
	}
	return NULL;
}

long content_length(struct HTTPRequest *req) {
	char *val;
	long len;

	val = lookup_header_field_value(req, "Content-Length");
	if (!val)
		return 0;
	len = atol(val);
	if (len < 0)
		log_exit("negative Content-length value");
	return len;
}

struct HTTPRequest* read_request(FILE *in) {
	struct HTTPRequest *req;
	struct HTTPHeaderField *h;

	req = xmalloc(sizeof(struct HTTPRequest));
	read_request_line(req, in);
	req->header = NULL;
	while ((h = read_header_field(in))) {
		h->next = req->header;
		req->header = h;
	}
	req->length = content_length(req);
	if (req->length != 0) {
		if (req->length > MAX_REQUEST_BODY_LENGTH)
			log_exit("failed to read request body");
		req->body = xmalloc(req->length);
		if (fread(req->body, req->length, 1, in) < 1)
			log_exit("failed to read request body");
	}
	else {
		req->body = NULL;
	}
	return req;
}

// HTTP 요청에 대한 응답을 반환하는 함수
void respond_to(struct HTTPRequest *req, FILE *out, char *docroot) {
	if (strcmp(req->method, "GET") == 0)
		do_file_response(req, out, docroot);
	else if (strcmp(req->method, "HEAD") == 0)
		do_file_response(req, out, docroot);
	else if (strcmp(req->method, "POST") == 0) {
		output_common_header_fields(req, out, "405 Method Not Allowed");
		// method_not_allowed(req, out);
		return ;
	}
	else {
		output_common_header_fields(req, out, "501 Not Implemented");
		// not_implemented(req, out);
		return ;
	}
}

// http의 동작 구조
void service(FILE *in, FILE *out, char *docroot) {
	struct HTTPRequest *req;

	// in 스트림에서 http 요청을 읽고
	// struct http request 구조체의 포인터에 저장하고 반환
	req = read_request(in);
	// 요청 req에 대한 응답을 두 번째 인자 out 스트림에 쓴다
	respond_to(req, out, docroot);
	free_request(req);
}

// 모든 응답에서 공통의 헤더를 출력하는 함수
void output_common_header_fields(struct HTTPRequest *req, FILE *out, char *status) {
	time_t t;
	struct tm *tm;
	char buf[TIME_BUF_SIZE];

	t = time(NULL);
	tm = gmtime(&t);
	if (!tm)
		log_exit("gmtime() failed: %s", strerror(errno));
	strftime(buf, TIME_BUF_SIZE, "%a, %d %b %Y %H:%M:%S GMT", tm);
	fprintf(out, "HTTP/1.%d %s\r\n", HTTP_MINOR_VERSION, status);
	fprintf(out, "Date: %s\r\n", buf);
	fprintf(out, "Server: %s/%s\r\n", SERVER_NAME, SERVER_VERSION);
	fprintf(out, "Connetion: close\r\n");
}

// GET 요청 처리
void do_file_response(struct HTTPRequest *req, FILE *out, char *docroot) {
	struct FileInfo *info;

	// get_fileinfo...??
	info = get_fieldinfo(docroot, req->path);
	if (!info->ok) {
		free_fileinfo(info);
		// not_found(req, out);
		return ;
	}
	output_common_header_fields(req, out, "200 OK");
	fprintf(out, "Content-Length: %ld\r\n", info->size);
	// fprintf(out, "Content-Type: %s\r\n", );
	fprintf(out, "\r\n");
	if (strcmp(req->method, "HEAD") != 0) {
		int fd;
		char buf[BLOCK_BUF_SIZE];
		ssize_t n;

		fd = open(info->path, O_RDONLY);
		if (fd < 0)
			log_exit("failed to open %s: %s", info->path, strerror(errno));
		for (;;) {
			n = read(fd, buf, BLOCK_BUF_SIZE);
			if (n < 0)
				log_exit("failed to read %s: %s", info->path, strerror(errno));
			if (n == 0)
				break;
			if (fwrite(buf, 1, n, out) < n)
				log_exit("failed to write to socket: %s", strerror(errno));
		}
		close(fd);
	}
	fflush(out);
	free_fileinfo(info);
}

int main(int argc, char *argv[])
{
    int server_fd;
    char *port = NULL;
    char *docroot;
    int do_chroot = 0;
    char *user = NULL;
    char *group = NULL;
    int opt;

    while ((opt = getopt_long(argc, argv, "", longopts, NULL)) != -1) {
        switch (opt) {
        case 0:
            break;
        case 'c':
            do_chroot = 1;
            break;
        case 'u':
            user = optarg;
            break;
        case 'g':
            group = optarg;
            break;
        case 'p':
            port = optarg;
            break;
        case 'h':
            fprintf(stdout, USAGE, argv[0]);
            exit(0);
        case '?':
            fprintf(stderr, USAGE, argv[0]);
            exit(1);
        }
    }
    if (optind != argc - 1) {
        fprintf(stderr, USAGE, argv[0]);
        exit(1);
    }
    docroot = argv[optind];

    if (do_chroot) {
        setup_environment(docroot, user, group);
        docroot = "";
    }
    install_signal_handlers();
    server_fd = listen_socket(port);
    if (!debug_mode) {
        openlog(SERVER_NAME, LOG_PID|LOG_NDELAY, LOG_DAEMON);
        become_daemon();
    }
	// server 시작
    server_main(server_fd, docroot);
    exit(0);
}

//=================================================//

int listen_socket(char *port)
{
    struct addrinfo hints, *res, *ai;
    int err;

    memset(&hints, 0, sizeof(struct addrinfo));
	// IPv4에 한정
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
	// AI_PASSIVE 소켓을 서버용으로 사용하기
    hints.ai_flags = AI_PASSIVE;
    if ((err = getaddrinfo(NULL, port, &hints, &res)) != 0)
        log_exit(gai_strerror(err));
    for (ai = res; ai; ai = ai->ai_next) {
        int sock;

        sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sock < 0) continue;
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            close(sock);
            continue;
        }
        if (listen(sock, MAX_BACKLOG) < 0) {
            close(sock);
            continue;
        }
        freeaddrinfo(res);
        return sock;
    }
    log_exit("failed to listen socket");
    return -1;  /* NOT REACH */
}


void server_main(int server_fd, char *docroot)
{
    for (;;) {
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof addr;
        int sock;
        int pid;

        sock = accept(server_fd, (struct sockaddr*)&addr, &addrlen);
        if (sock < 0)
			log_exit("accept(2) failed: %s", strerror(errno));
        pid = fork();
        if (pid < 0)
			exit(3);
        if (pid == 0) {   /* child */
            FILE *inf = fdopen(sock, "r");
            FILE *outf = fdopen(sock, "w");

            service(inf, outf, docroot);
            exit(0);
        }
        close(sock);
    }
}

void detach_children(void)
{
    struct sigaction act;

    act.sa_handler = noop_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART | SA_NOCLDWAIT;
	// wait()을 안할 것이라고 커널에 선언.
	// 자식 프로세스를 좀비로 만들지 않는다.
    if (sigaction(SIGCHLD, &act, NULL) < 0) {
        log_exit("sigaction() failed: %s", strerror(errno));
    }
}

void noop_handler(int sig)
{
    ;
}
