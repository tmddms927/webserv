#include "Server.hpp"

/*
** check client request header 
*/
void Server::checkReqHeader() {
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
	std::string uri = clients[curr_event->ident].getURI();
	if (uri[0] != '/')
		clients[curr_event->ident].setURI("/" + uri);
	if (findServerLocationIndex_findRoot())
		return ;
	if (findServerLocationIndex_findServerBlock1())
		return ;
	// server block location과 uri가 맞지 않은 경우
	clients[curr_event->ident].setResLocationIndex(0);
	clients[curr_event->ident].setResponseFileDirectory(\
			config[clients[curr_event->ident].getResServerBlockIndex()].location[0].location_root\
			+ config[clients[curr_event->ident].getResServerBlockIndex()].location[0].err_page);
	clients[curr_event->ident].setStatus(404);
}

/*
** findServerLocationIndex_findRoot
** /인지 확인
*/
bool Server::findServerLocationIndex_findRoot() {
	size_t found;
	std::string uri = clients[curr_event->ident].getURI();

	found = uri.find("/");
	if (uri.length() == 1) {
		clients[curr_event->ident].setResponseFileDirectory(\
			config[clients[curr_event->ident].getResServerBlockIndex()].location[0].location_root\
			+ "/" + config[clients[curr_event->ident].getResServerBlockIndex()].location[0].index);
		return true;
	}
	return false;
}

/*
** findServerLocationIndex_findServerBlock
*/
bool Server::findServerLocationIndex_findServerBlock1() {
	size_t found;
	int server_block = clients[curr_event->ident].getResServerBlockIndex();
	std::string uri = clients[curr_event->ident].getURI();
	std::string temp = uri.substr(1);
	found = temp.find("/");

	if (found == std::string::npos)
		return findServerLocationIndex_findServerBlock2();
	else
		return findServerLocationIndex_findServerBlock3();
	return false;
}

/*
** findServerLocationIndex
** 127.0.0.1/abc 일 경우
*/
bool Server::findServerLocationIndex_findServerBlock2() {
	int server_block = clients[curr_event->ident].getResServerBlockIndex();
	std::string uri = clients[curr_event->ident].getURI();
	int size = config[server_block].location.size();

	for (int i = 1; i < size; ++i) {
		if (uri + "/" == config[server_block].location[i].location_uri) {
			clients[curr_event->ident].setResLocationIndex(i);
			clients[curr_event->ident].setResponseFileDirectory(\
				config[server_block].location[i].location_root + "/"\
				 + config[server_block].location[i].index);
			return true;
		}
	}
	clients[curr_event->ident].setResponseFileDirectory(\
		config[server_block].location[0].location_root + uri);
	isFile();
	return true;
}

/*
** findServerLocationIndex
** 127.0.0.1/abc/ 일 경우
*/
bool Server::findServerLocationIndex_findServerBlock3() {
	int server_block = clients[curr_event->ident].getResServerBlockIndex();
	int size = config[server_block].location.size();
	std::string uri = clients[curr_event->ident].getURI();
	std::string temp = uri.substr(1);
	size_t found = temp.find("/");
	temp = uri.substr(0, found + 2);
	std::string sub = uri.substr(found + 1);

	for (int i = 1; i < size; ++i) {
		if (temp == config[server_block].location[i].location_uri) {
			clients[curr_event->ident].setResLocationIndex(i);
			clients[curr_event->ident].setResponseFileDirectory(\
				config[server_block].location[i].location_root + sub);
			if (clients[curr_event->ident].getMethod() != PUT_BIT && \
			clients[curr_event->ident].getMethod() != POST_BIT)
				isFile();
			return findServerLocationIndex_checkAsterisk(sub);
		}
	}
	return false;
}

/*
** server block location - asterisk check
*/
bool Server::findServerLocationIndex_checkAsterisk(std::string const & str) {
	int server_block = clients[curr_event->ident].getResServerBlockIndex();
	int location_index = clients[curr_event->ident].getResLocationIndex();
	std::string uri;
	size_t found;

	if (config[server_block].location[0].is_aster == true)
		return true;
	if (config[server_block].location[location_index].is_aster != true) {
		uri = str.substr(1);
		found = uri.find("/");
		uri = uri.substr(found + 1);
		if (found != std::string::npos && uri.length() != 0) {
			clients[curr_event->ident].setResLocationIndex(0);
			clients[curr_event->ident].setStatus(404);
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

	if (stat(path.c_str(), &ss) == -1) {
		clients[curr_event->ident].setStatus(404);
	}
	else if (S_ISDIR(ss.st_mode)) {
		if (path == config[server_block_index].location[location_index].location_root) {
			clients[curr_event->ident].setResponseFileDirectory(path +\
				config[server_block_index].location[location_index].index);
		}
		else {
			if (path[path.length() - 1] == '/')
				clients[curr_event->ident].setResponseFileDirectory(path +\
					config[server_block_index].location[location_index].err_page);
			else
				clients[curr_event->ident].setResponseFileDirectory(path + "/" +\
					config[server_block_index].location[location_index].err_page);
		}
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
