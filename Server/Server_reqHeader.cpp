#include "Server.hpp"

/*
** check client request header 
*/
void Server::checkReqHeader() {
	std::cout << static_cast<int>(clients[curr_event->ident].getMethod()) << std::endl;
	findServerBlockIndex();
	findServerLocationIndex();
	checkAllowedMethod();
}

/*
** server block의 인덱스 찾아서 저장
*/
void Server::findServerBlockIndex() {
	uintptr_t server_fd = clients[curr_event->ident].getServerFd();
	size_t size = server_socket.size();

	for (size_t i = 0; i < size; ++i) {
		if (server_socket[i] == server_fd) {
			clients[curr_event->ident].setResServerBlockIndex(i);
			return ;
		}
	}
}

/*
** server block에 맞는 location 찾아서 저장
*/
void Server::findServerLocationIndex() {
	if (findServerLocationIndex_findRoot())
		return ;
	if (findServerLocationIndex_findServerBlock1())
		return ;
	if (findServerLocationIndex_findServerBlock2())
		return ;
	if (findServerLocationIndex_findServerBlock3())
		return ;
	// server block location과 uri가 맞지 않은 경우
	clients[curr_event->ident].setResLocationIndex(0);
	clients[curr_event->ident].setResponseFileDirectory(\
			config[clients[curr_event->ident].getResServerBlockIndex()].location[0].location_root\
			+ config[clients[curr_event->ident].getResServerBlockIndex()].location[0].err_page);
	clients[curr_event->ident].setStatus(404);
}

/*
** findServerLocationIndex_1
** 처음에 / 없는지 체크
*/
bool Server::findServerLocationIndex_findRoot() {
	size_t found;
	std::string uri = clients[curr_event->ident].getURI();

	found = uri.find("/");
	if (found == std::string::npos || found != 0) {
		clients[curr_event->ident].setStatus(404);
		return true;
	}
	return false;
}

/*
** findServerLocationIndex_1
** /인지 확인
*/
bool Server::findServerLocationIndex_findServerBlock1() {
	std::string uri = clients[curr_event->ident].getURI();
	std::string temp = uri.substr(1);

	// root directory
	if (temp.length() == 0) {
		clients[curr_event->ident].setResponseFileDirectory(\
			config[clients[curr_event->ident].getResServerBlockIndex()].location[0].location_root\
			+ temp + config[clients[curr_event->ident].getResServerBlockIndex()].location[0].index);
		return true;
	}
	return false;
}

/*
** findServerLocationIndex
** / ~이면 ~가 location인지 확인
** 아니면 root + ~
*/
bool Server::findServerLocationIndex_findServerBlock2() {
	std::string uri = clients[curr_event->ident].getURI();
	std::string temp = uri.substr(1);
	int server_block = clients[curr_event->ident].getResServerBlockIndex();
	int index = checkLocation(server_block,\
			config[clients[curr_event->ident].getResServerBlockIndex()].location.size(), temp);

	if (index == -1) {
		clients[curr_event->ident].setResponseFileDirectory(\
			config[server_block].location[0].location_root + temp);
		return true;
	}
	else {
		clients[curr_event->ident].setResponseFileDirectory(\
			config[server_block].location[index].location_root + \
			config[server_block].location[index].index);
		return true;
	}
	return false;
}

/*
** findServerLocationIndex
** /~/이면 ~가 location인지 확인
*/
bool Server::findServerLocationIndex_findServerBlock3() {
	return true;
	// size_t found;
	// int server_block = clients[curr_event->ident].getResServerBlockIndex();
	// int size = config[server_block].location.size();
	// std::string uri = clients[curr_event->ident].getURI();
	// std::string temp = uri.substr(1);
	// found = temp.find("/");

	// // server block이 있을 경우
	// temp = uri.substr(0, found + 1);
	// for (int i = 1; i < size; ++i) {
	// 	if (temp == config[server_block].location[i].location_uri) {
	// 		clients[curr_event->ident].setResLocationIndex(i);
	// 		clients[curr_event->ident].setResponseFileDirectory(\
	// 			config[server_block].location[i].location_root + uri);
	// 		if (!findServerLocationIndex_checkAsterisk())
	// 			return false;
	// 		isFile();
	// 		return true;
	// 	}
	// }
	// return false;
}

/*
** server block location - asterisk check
*/
bool Server::findServerLocationIndex_checkAsterisk() {
	int server_block = clients[curr_event->ident].getResServerBlockIndex();
	int location_index = clients[curr_event->ident].getResLocationIndex();

	if (config[server_block].location[location_index].is_aster != true) {
		std::string uri = clients[curr_event->ident].getURI();
		size_t found = uri.find(config[server_block].location[location_index].location_uri);
		uri = uri.substr(found);

		if (uri.find("/") != std::string::npos) {
			clients[curr_event->ident].setResLocationIndex(0);
			return false;
		}
	}
	return true;
}

/*
** str이 location인지 확인
*/
int Server::checkLocation(int const & sb, int const & size, std::string const & str) {
	std::string end = "";

	if (str[str.size() - 1] != '/')
		end = "/";
	for (int i = 1; i < size; ++i) {
		if (str + end == config[sb].location[i].location_uri)
			return i;
	}
	return -1;
}

/*
** directory이면 index file 붙여주기
*/
void Server::isFile() {
	std::string path = clients[curr_event->ident].getResponseFileDirectory();
	int server_block_index = clients[curr_event->ident].getResServerBlockIndex();
	int location_index = clients[curr_event->ident].getResLocationIndex();
	struct stat ss;

	if (stat(path.c_str(), &ss) == -1)
		clients[curr_event->ident].setStatus(404);
	else if (S_ISDIR(ss.st_mode)) {
		if (config[server_block_index].location[location_index].index == "")
			clients[curr_event->ident].setStatus(404);
		else
			clients[curr_event->ident].setResponseFileDirectory(path +
				config[server_block_index].location[location_index].index);
	}
}

/*
** allowed method check
*/
void Server::checkAllowedMethod() {
	char a = clients[curr_event->ident].getMethod();
	int block = clients[curr_event->ident].getResServerBlockIndex();
	int location = clients[curr_event->ident].getResLocationIndex();

	if ((config[block].location[location].allowed_method & a) == GET_BIT)
		return ;
	if ((config[block].location[location].allowed_method & a) == POST_BIT)
		return ;
	if ((config[block].location[location].allowed_method & a) == DELETE_BIT)
		return ;
	if ((config[block].location[location].allowed_method & a) == PUT_BIT)
		return ;
	if ((config[block].location[location].allowed_method & a) == HEAD_BIT)
		return ;
	// 만약 default method가 있다면...?
	clients[curr_event->ident].setStatus(405);
}
