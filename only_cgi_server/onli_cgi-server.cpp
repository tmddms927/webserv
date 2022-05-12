#include <iostream>
#include <unistd.h>
#include <sys/event.h>
#include <vector>
#include <fcntl.h>
#include <signal.h>
#include <sstream>
#include "HTTP.hpp"

#define READ                    0
#define WRITE                   1
#define CONTENT_SIZE            30
#define READ_BUF_SIZE           200
#define KQUEUE_EVENT_LIST_SIZE  1024

std::string     input = "hi im jeokim";

typedef struct kevent   t_kevent;

class KQueue {
private:
    int kq;
    t_kevent					event_list[KQUEUE_EVENT_LIST_SIZE];
	t_kevent*					curr_event;
    std::vector<struct kevent>		change_list;

public:
    KQueue();

    void            kqueueInit();
    int             kqueueEvent();
    t_kevent*       getEventList();
    void	        change_events(uintptr_t const & ident,
			                        int16_t const & filter, uint16_t const & flags);
};

KQueue::KQueue() {

}


void    KQueue::kqueueInit() {
	kq = kqueue();
	if (kq == -1)
		throw "kqueue() error!";
}

int     KQueue::kqueueEvent() {
	int new_events;

	new_events = kevent(kq, &change_list[0], change_list.size(),
				        event_list, KQUEUE_EVENT_LIST_SIZE, NULL);
	change_list.clear();

    return new_events;
}

void KQueue::change_events(uintptr_t const & ident, int16_t const & filter,
								uint16_t const & flags)
{
	t_kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, 0, 0, NULL);
	change_list.push_back(temp_event);
}

t_kevent*   KQueue::getEventList() {
    return static_cast<t_kevent*>(event_list);
}

//
void    set_CGI_read_fd(int pipe[2]) {
    close(pipe[WRITE]);
    dup2(pipe[READ], STDIN_FILENO);
    close(pipe[READ]);
}

void    set_CGI_write_fd(int pipe[2]) {
    close(pipe[READ]);
    dup2(pipe[WRITE], STDOUT_FILENO);
    close(pipe[WRITE]);
}

int    cgi_fork(KQueue & kq) {
    pid_t   pid;
    int     server_read_pipe[2];
    int     server_write_pipe[2];

    char    *arg[2];
    char    *env[5];

    //file open, read, write error체크 엄밀히 하기
    pipe(server_read_pipe);
    pipe(server_write_pipe);

    pid = fork();
    if (pid < 0)
        ;
    else if (pid == 0) {    //  << CGI(child) >>
        //  pipe setting
        set_CGI_read_fd(server_write_pipe); // server 기준 write, cgi 기준 read
        set_CGI_write_fd(server_read_pipe);
        //  arg, env 세팅
        arg[0] = "cgi_tester";
        arg[1] = NULL;
        env[0] = "REQUEST_METHOD=POST";
        // env[1] = "REDIRECT_STATUS=200";
        env[1] = "SERVER_PROTOCOL=HTTP/1.1";
        env[2] = "PATH_INFO=/Users/hwan/Documents/webserv/hello";
        std::stringstream ss;
        // ss << input.size();
        ss << CONTENT_SIZE;
        std::string s = "CONTENT_LENGTH=";
        s = s + ss.str();
        env[3] = const_cast<char*>(s.c_str());
        env[4] = NULL;
        ::execve("cgi_tester",arg , env);
    }
    else {                  //  << Server(parent) >>
        //  pipe setting
        waitpid(pid, 0, WNOWAIT);
        std::cout << "server" << std::endl;
        close(server_write_pipe[READ]);
        close(server_read_pipe[WRITE]);
        fcntl(server_write_pipe[WRITE], F_SETFL, O_NONBLOCK);
        fcntl(server_read_pipe[READ], F_SETFL, O_NONBLOCK);
        //  regist CGI fd to variable
        std::cout << server_write_pipe[WRITE] << ", " << server_read_pipe[READ] << std::endl;

        return pid;
    }
}

void    request(uintptr_t fd) {
    int wr;
    wr = write(fd, input.c_str(), input.size());
}

void    response(uintptr_t fd, KQueue kq, pid_t pid) {
    int rd;

    char buf[READ_BUF_SIZE];
    memset(buf, 0, READ_BUF_SIZE);
    rd = read(fd, buf, READ_BUF_SIZE - 1);
    if (rd < 0)
        std::cout << "no buf to read" << std::endl;
    else if (rd == 0) {
        std::cout << "something problem in read fd" << std::endl;
        close(fd);
    }
    else {
        std::cout << "||       << response >>        ||" << std::endl;
        std::cout << buf << "<--buf end>>" << std::endl;
    }
}

void    sigpipe(int) {
    std::cout << "sigpipe" << std::endl;
    exit(1);
}

void    sigchild(int) {
    int status;
    std::cout << "sigchild" << std::endl;
    wait(&status);
}

int main() {
    KQueue kq;
    pid_t pid;
    HTTP    http;
    std::string buf;

    signal(SIGPIPE, sigpipe);
    signal(SIGCHLD, sigchild);
    kq.kqueueInit();
    http.reqInputBuf(buf);
    if (!http.reqCheckFinished())
        std::cout << "request not finished" << std::endl;

    uintptr_t read_fd;
    uintptr_t write_fd;
    pid_t   pid;
    http.cgi_creat(&write_fd, &read_fd, &pid);
    kq.change_events(write_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
    kq.change_events(read_fd, EVFILT_READ, EV_ADD | EV_ENABLE);

    while (1) {
        int event_count;
        std::cout << "while start" << std::endl;
        // HTTP에 request 입력
        event_count = kq.kqueueEvent();
        std::cout << "event_cout : " << event_count << std::endl;
        for (int idx = 0; idx < event_count; idx++) {
            if (kq.getEventList()[idx].filter == EVFILT_READ)
                http.cgi_read();
            else if (kq.getEventList()[idx].filter == EVFILT_WRITE) {
                if (kq.getEventList()[idx].ident == write_fd)
                    http.cgi_write();
                else
                    ;
            }
        }
    }
    return 0;
}