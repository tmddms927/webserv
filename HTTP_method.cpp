
#include "Server.hpp"
#include "unistd.h"

int	HTTP::GETMethod(uintptr_t & fd, std::string & body) {
	char	buf[SOCKET_READ_BUF];
	int		read_len;

	std::memset(buf, 0, SOCKET_READ_BUF);
	read_len = read(fd, buf, SOCKET_READ_BUF - 1);
	if (read_len == 0) {
		std::cerr << "client read error!" << std::endl;
		return -1;
	}
	else if (read_len < 0) {
        return 1;
    }
    body += buf;
    std::cout << "GET Method " << "[" << buf << "]" << std::endl;
	return 0;
}

int	HTTP::POSTMethod(uintptr_t & fd, std::string & body) {
	// char	buf[SOCKET_READ_BUF];
	int		written_len;

	// std::memset(buf, 0, SOCKET_READ_BUF);
	std::cout << "POST body : " << body << std::endl;
	// written_len = write(fd, body.c_str(), SOCKET_READ_BUF - 1);
	written_len = write(fd, body.c_str(), body.size());

	if (written_len == 0) {
		std::cerr << "client read error!" << std::endl;
		return -1;
	}
	else if (written_len < 0)
		return 1;
	return 0;
}
