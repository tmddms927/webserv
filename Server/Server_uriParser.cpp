#include "Server.hpp"

/*
** find server block
** 아예 수정해야 됨!
*/
void Server::uriParser() {
	int size = config.size();
	size_t found;
	std::string uri = clients[curr_event->ident].getURI();

	// 처음에 / 없을 때
	found = uri.find("/");
    if (found == std::string::npos || found != 0) {
		clients[curr_event->ident].setStatus(404);
        return ;
	}

	// server block이 없을 경우
    std::string temp = uri.substr(1);
	found = temp.find("/");
    if (found == std::string::npos) {
		clients[curr_event->ident].setResponseFileDirectory(config[0].root + uri);
		isFile();
        return ;
	}

    // server block이 있을 경우
	temp = uri.substr(0, found + 1);
	for (int i = 1; i < size; ++i) {
		if (server_socket[i] == clients[curr_event->ident].getServerFd()) {
			if (temp == config[i].location) {
				clients[curr_event->ident].setResponseFileDirectory(config[i].root + uri);
				isFile();
				return ;
			}
		}
	}
    // server block과 uri, port가 맞지 않은 경우
    clients[curr_event->ident].setStatus(404);
}

/*
** directory이면 index file 붙여주기
*/
void Server::isFile() {
	std::string path = clients[curr_event->ident].getResponseFileDirectory();
	struct stat ss;

	if (stat(path.c_str(), &ss) == -1) {
		clients[curr_event->ident].setStatus(404);
	}
	else if (S_ISDIR(ss.st_mode)) {
		if (global_config.index == "")
			clients[curr_event->ident].setStatus(200);
		else
			clients[curr_event->ident].setResponseFileDirectory(path + global_config.index);
	}
}
