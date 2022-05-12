#include "../Server/Server.hpp"
#include "../CGInterface/CGInterface.hpp"
#include <unistd.h>
#include <vector>
#include <sys/event.h>
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

void    CGInterface::CGI_fork(struct s_cgiInfo *ci) {
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
        env[0] = "REQUEST_METHOD=POST";     //method
        // env[1] = "REDIRECT_STATUS=200";  // php option
        env[1] = "SERVER_PROTOCOL=HTTP/1.1";
        env[2] = "PATH_INFO=/Users/hwan/Documents/webserv/hello";
        env[3] = "CONTENT_LENGTH=10";
        env[4] = NULL;
        ::execve("cgi_tester",arg , env);
    }
    else {                  //  << Server(parent) >>
        //  pipe setting
        close(server_write_pipe[READ]);
        close(server_read_pipe[WRITE]);
        fcntl(server_write_pipe[WRITE], F_SETFL, O_NONBLOCK);
        fcntl(server_read_pipe[READ], F_SETFL, O_NONBLOCK);
        ci->read_fd = server_read_pipe[READ];
        ci->write_fd = server_write_pipe[WRITE];
        ci->pid = pid;
    }
}

bool    CGInterface::CGI_write(std::string const &body) {
    int wr;

    wr = write(write_fd, body.c_str(), body.size());
    if (written_len += wr) {
        if (written_len >= cgi_write_len) {
            close(write_fd);
            return true;
        }
    }
    return false;
}

bool    CGInterface::CGI_read(std::string & buf, size_t buf_size) {
    int     rd;
    size_t  found = 0;
    std::string s;
    std::stringstream ss;
    char    tmp[buf_size + 1];

    memset(tmp, 0, buf_size + 1);
    rd = read(read_fd, tmp, buf_size);
    if (rd > 0) {
        ss << tmp;
        buf += ss.str();
    }
    if (read_len == std::string::npos) {
        found = buf.find("\r\n\r\n");
        if (found != std::string::npos)
            return false;
    }
    read_len += rd - found;
    if (read_len >= cgi_write_len)
        return true;
    return false;
}
