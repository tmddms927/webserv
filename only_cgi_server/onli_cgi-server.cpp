#include <unistd.h>
#include <sys/event.h>
#define READ    0
#define WRITE   1

//
void    set_CGI_read_fd(int pipe[2]) {
    close(pipe[WRITE]);
    dup2(pipe[READ], STDIN_FILENO);
    close(pipe[READ]);
    //nonblock..?
}

void    set_CGI_write_fd(int pipe[2]) {
    close(pipe[READ]);
    dup2(pipe[WRITE], STDOUT_FILENO);
    close(pipe[WRITE]);
    //nonblock..?
}

int main() {
    pid_t   pid;
    int     server_read_pipe[2];
    int     server_write_pipe[2];

    char    *arg[2];
    char    *env[5];

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
        env[0] = "REQUEST_METHOD=GET";
        env[1] = "REDIRECT_STATUS=200";
        env[2] = "SERVER_PROTOCOL=HTTP/1.1";
        env[3] = "PATH_INFO=/Users/gilee/CLionProjects/cgi/CMakeLists.txt";
        env[4] = NULL;
        ::execve("cgi_tester",arg , env);
    }
    else {                  //  << Server(parent) >>
        //  pipe setting
        close(server_write_pipe[READ]);
        close(server_read_pipe[WRITE]);
        //  regist CGI fd to variable
        int kq;
        kq = kqueue();
	    if (kq == -1)
		    throw "kqueue() error!";
        //  regist event to variable
    }
}