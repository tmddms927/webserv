#include "../Server/Server.hpp"
#include "../CGIInterface/CGIInterface.hpp"
#include <unistd.h>
#include <vector>
#include <sys/event.h>
#include <string>
#include <sstream>

#define READ    0
#define WRITE   1

int    set_CGI_read_fd(int pipe[2]) {
	if (close(pipe[WRITE]) == -1 ||
		dup2(pipe[READ], STDIN_FILENO) == -1 ||
		close(pipe[READ]) == -1)
		return -1;
	return 0;
}

int    set_CGI_write_fd(int pipe[2]) {
	if (close(pipe[READ]) == -1 ||
		dup2(pipe[WRITE], STDOUT_FILENO) == -1 ||
		close(pipe[WRITE]) == -1)
		return -1;
	return 0;
}

int    CGIInterface::CGI_fork(struct s_cgiInfo & ci,
	std::vector<std::string> & arg_v, std::vector<std::string> & env_v) {
	pid_t   pid;
	int     server_read_pipe[2];
	int     server_write_pipe[2];
	char	**arg;
	char	**env;


	//file open, read, write error체크 엄밀히 하기
	if (pipe(server_read_pipe) == -1 ||
		pipe(server_write_pipe) == -1)
		return CGI_ERROR;

	pid = fork();
	if (pid < 0)
		return CGI_ERROR;
	else if (pid == 0) {    //  << CGI(child) >>
		arg = new char*[arg_v.size() + 1];
		env = new char*[env_v.size() + 1];

		memset(arg, 0, sizeof(char *) * (arg_v.size() + 1));
		memset(env, 0, sizeof(char *) * (env_v.size() + 1));


	int i = 0;
	for (std::vector<std::string>::iterator
			it = arg_v.begin(); it != arg_v.end(); it++) {
				// arg[i] = new char[it->size() + 1];
				// memset(arg[i], 0, it->size() + 1);
				// for (size_t j = 0; j < it->size(); j++)
				// 	arg[i][j] = it->c_str()[j];
				// i++;
				arg[i++] = ft_to_c_str(*it);
			}
	i = 0;
	for (std::vector<std::string>::iterator
			it = env_v.begin(); it != env_v.end(); it++) {
				// env[i] = new char[it->size() + 1];
				// memset(env[i], 0, it->size() + 1);
				// for (size_t j = 0; j < it->size(); j++)
				// 	env[i][j] = it->c_str()[j];
				// i++;
				env[i++] = ft_to_c_str(*it);
			}
				
		//  pipe setting
		if (set_CGI_read_fd(server_write_pipe) == -1 || // server 기준 write, cgi 기준 read
			set_CGI_write_fd(server_read_pipe) == -1) {
				std::cout << "CGI fork fail" << std::endl;
			exit(1);
			}
		::execve(arg[0], arg , env);
	}
                  //  << Server(parent) >>
	//  pipe setting
	if (close(server_write_pipe[READ]) == -1 ||
		close(server_read_pipe[WRITE]) == -1)
		return CGI_ERROR;
	if (fcntl(server_write_pipe[WRITE], F_SETFL, O_NONBLOCK) == -1 ||
		fcntl(server_read_pipe[READ], F_SETFL, O_NONBLOCK) == -1)
		return CGI_ERROR;
	read_fd = server_read_pipe[READ];
	write_fd = server_write_pipe[WRITE];
	ci.read_fd = server_read_pipe[READ];
	ci.write_fd = server_write_pipe[WRITE];
	ci.pid = pid;
	return CGI_FINISHED;
}

int    CGIInterface::CGI_write(std::string const &body) {
	ssize_t wr = 0;
	size_t length = body.size();
	size_t write_size;

	if (RW_MAX_SIZE  > length)
		write_size = length;
	else
		write_size = RW_MAX_SIZE;

	wr = write(write_fd, body.c_str(), write_size);
	if (wr != -1)
		written_length += wr;
	return wr;
}

int    CGIInterface::CGI_read(std::string & buf) {
	int rd = RW_MAX_SIZE + 1;
	char tmp[RW_MAX_SIZE + 1];

	memset(tmp, 0, RW_MAX_SIZE + 1);

	rd = read(read_fd, tmp, RW_MAX_SIZE);
	if (rd > 0)
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

int     CGIInterface::CGI_getReadFd() {
	return read_fd;
}

int     CGIInterface::CGI_getWriteFd() {
	return write_fd;
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
