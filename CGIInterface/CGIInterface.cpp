#include "../Server/Server.hpp"
#include "../CGIInterface/CGIInterface.hpp"
#include <unistd.h>
#include <vector>
#include <sys/event.h>
#include <string>
#include <sstream>

#define READ    0
#define WRITE   1

/*
**  SIGPIPE 감지 시에 실행할 함수
*/
void    sigpipe(int) {
    std::cout << "sigpipe" << std::endl;
}

/*  
**  SIGCHLD 감지 시에 실행할 함수
**  종료됨을 알린 자식프로세스의 자원을 부모 프로세스가 회수 -> wait()
*/
void    sigchild(int) {
    int status;
    std::cout << "sigchild" << std::endl;
    wait(&status);
}

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

void    CGIInterface::CGI_fork(struct s_cgiInfo &ci ,struct s_cgiArg &ca) {
    pid_t   pid;
    int     server_read_pipe[2];
    int     server_write_pipe[2];
    char    *arg[2];
    char    *env[5];

    //file open, read, write error체크 엄밀히 하기
    if (pipe(server_read_pipe) == -1)
        std::cout << "pipe open error";
    if (pipe(server_write_pipe) == -1)
        std::cout << "pipe open error";

    pid = fork();
    if (pid < 0)
        ;
    else if (pid == 0) {    //  << CGI(child) >>
        //  pipe setting
        set_CGI_read_fd(server_write_pipe); // server 기준 write, cgi 기준 read
        set_CGI_write_fd(server_read_pipe);
        //  arg, env 세팅
        arg[0] = const_cast<char*>("cgi_tester");
        arg[1] = NULL;
        std::cout << ca.method_name;
        env[0] = const_cast<char*>(("REQUEST_METHOD=" + ca.method_name).c_str());     //method
        env[1] = const_cast<char*>("SERVER_PROTOCOL=HTTP/1.1");
        env[2] = const_cast<char*>("PATH_INFO=/Users/hwan/Documents/webserv/hello");
        env[3] = const_cast<char*>(("CONTENT_LENGTH=" + ft_itoa(ca.content_length)).c_str());
        env[4] = NULL;
        ::execve("cgi_tester",arg , env);
    }
    else {                  //  << Server(parent) >>
        //  pipe setting
        close(server_write_pipe[READ]);
        close(server_read_pipe[WRITE]);
        fcntl(server_write_pipe[WRITE], F_SETFL, O_NONBLOCK);
        fcntl(server_read_pipe[READ], F_SETFL, O_NONBLOCK);
        std::cout << "hihi" << std::endl;
        read_fd = server_read_pipe[READ];
        write_fd = server_write_pipe[WRITE];
        ci.read_fd = server_read_pipe[READ];
        ci.write_fd = server_write_pipe[WRITE];
        ci.pid = pid;
    }
}

int    CGIInterface::CGI_write(std::string const &body) {
    ssize_t wr;

    wr = write(write_fd, body.c_str(), body.size());
    if (wr > 0)
        written_length += wr;
    return wr;
}

int    CGIInterface::CGI_read(std::string & buf, size_t buf_size) {
    int rd;
    char tmp[buf_size + 1];

    memset(tmp, 0, buf_size + 1);
    rd = read(read_fd, tmp, buf_size);
    std::cout << "tmp : " << tmp << std::endl;
    if (rd <= 0)
        std::cout << "CGI_read error" << std::endl;
    std::cout << "error" << std::endl;
    buf = tmp;
    return rd;
}

size_t  CGIInterface::CGI_getWrittenLength() {
    return written_length;
}

void  CGIInterface::CGI_clear() {
    write_fd = 0;
    read_fd = 0;
    written_length = 0;
}

CGIInterface::CGIInterface(/* args */): written_length(0) {
    // std::cout << "CGIInterface default constructor called" << std::endl;
}

CGIInterface::~CGIInterface() {
    // std::cout << "CGIInterface default destructor called" << std::endl;
}

// CGIInterface::CGIInterface(CGIInterface const &) {
//     // std::cout << "CGIInterface copy constructor called" << std::endl;

// }
// CGIInterface & CGIInterface::operator=(CGIInterface const &) {
//     // std::cout << "CGIInterface destructor called" << std::endl;
//     return *this;
// }

int     CGIInterface::CGI_getReadFd() {
    return read_fd;
}

int     CGIInterface::CGI_getWriteFd() {
    return write_fd;
}