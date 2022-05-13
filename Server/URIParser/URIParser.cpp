#include "URIParser.hpp"

URIParser::URIParser(HTTP & _client, std::vector<uintptr_t> const & _server_socket,\
						std::vector<servers> const & _config)
		: client(_client), server_socket(_server_socket), config(_config) {

}

URIParser::~URIParser() {

}

/*
** check client request header 
*/
void URIParser::checkReqHeader() {
	findServerBlockIndex();
	findServerLocationIndex();
	checkURICGI();
	checkAllowedMethod();
}

/*
** server block의 인덱스 찾아서 저장
*/
void URIParser::findServerBlockIndex() {
	uintptr_t server_fd = client.getServerFd();
	size_t size = server_socket.size();

	for (size_t i = 0; i < size; ++i) {
		if (server_socket[i] == server_fd) {
			client.setResServerBlockIndex(i);
			return ;
		}
	}
}

/*
** server block에 맞는 location 찾아서 저장
*/
void URIParser::findServerLocationIndex() {
	std::string uri = client.getURI();
	if (uri[0] != '/')
		client.setURI("/" + uri);
	if (findServerLocationIndex_findRoot())
		return ;
	if (findServerLocationIndex_findServerBlock1())
		return ;
	// server block location과 uri가 맞지 않은 경우
	client.setResLocationIndex(0);
	client.setResponseFileDirectory(\
			config[client.getResServerBlockIndex()].location[0].location_root\
			+ config[client.getResServerBlockIndex()].location[0].err_page);
	client.setStatus(404);
}

/*
** findServerLocationIndex_findRoot
** /인지 확인
*/
bool URIParser::findServerLocationIndex_findRoot() {
	size_t found;
	std::string uri = client.getURI();

	found = uri.find("/");
	if (uri.length() == 1) {
		client.setResponseFileDirectory(\
			config[client.getResServerBlockIndex()].location[0].location_root\
			+ "/" + config[client.getResServerBlockIndex()].location[0].index);
		return true;
	}
	return false;
}

/*
** findServerLocationIndex_findServerBlock
*/
bool URIParser::findServerLocationIndex_findServerBlock1() {
	size_t found;
	int server_block = client.getResServerBlockIndex();
	std::string uri = client.getURI();
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
bool URIParser::findServerLocationIndex_findServerBlock2() {
	int server_block = client.getResServerBlockIndex();
	std::string uri = client.getURI();
	int size = config[server_block].location.size();

	for (int i = 1; i < size; ++i) {
		if (uri + "/" == config[server_block].location[i].location_uri) {
			client.setResLocationIndex(i);
			client.setResponseFileDirectory(\
				config[server_block].location[i].location_root + "/"\
				 + config[server_block].location[i].index);
			return true;
		}
	}
	client.setResponseFileDirectory(\
		config[server_block].location[0].location_root + uri);
	isFile();
	return true;
}

/*
** findServerLocationIndex
** 127.0.0.1/abc/ 일 경우
*/
bool URIParser::findServerLocationIndex_findServerBlock3() {
	int server_block = client.getResServerBlockIndex();
	int size = config[server_block].location.size();
	std::string uri = client.getURI();
	std::string temp = uri.substr(1);
	size_t found = temp.find("/");
	temp = uri.substr(0, found + 2);
	std::string sub = uri.substr(found + 1);

	for (int i = 1; i < size; ++i) {
		if (temp == config[server_block].location[i].location_uri) {
			client.setResLocationIndex(i);
			client.setResponseFileDirectory(\
				config[server_block].location[i].location_root + sub);
			if (client.getMethod() != PUT_BIT && \
			client.getMethod() != POST_BIT)
				isFile();
			return findServerLocationIndex_checkAsterisk(sub);
		}
	}
	return false;
}

/*
** server block location - asterisk check
*/
bool URIParser::findServerLocationIndex_checkAsterisk(std::string const & str) {
	int server_block = client.getResServerBlockIndex();
	int location_index = client.getResLocationIndex();
	std::string uri;
	size_t found;

	if (config[server_block].location[0].is_aster == true)
		return true;
	if (config[server_block].location[location_index].is_aster != true) {
		uri = str.substr(1);
		found = uri.find("/");
		uri = uri.substr(found + 1);
		if (found != std::string::npos && uri.length() != 0) {
			client.setResLocationIndex(0);
			client.setStatus(404);
			return false;
		}
	}
	return true;
}

/*
** str이 location인지 확인
*/
int URIParser::checkLocation(int const & sb, int const & size, std::string const & str) {
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
void URIParser::isFile() {
	std::string path = client.getResponseFileDirectory();
	int server_block_index = client.getResServerBlockIndex();
	int location_index = client.getResLocationIndex();
	struct stat ss;

	if (stat(path.c_str(), &ss) == -1) {
		client.setStatus(404);
	}
	else if (S_ISDIR(ss.st_mode)) {
		if (path == config[server_block_index].location[location_index].location_root) {
			client.setResponseFileDirectory(path +\
				config[server_block_index].location[location_index].index);
		}
		else {
			if (path[path.length() - 1] == '/')
				client.setResponseFileDirectory(path +\
					config[server_block_index].location[location_index].err_page);
			else
				client.setResponseFileDirectory(path + "/" +\
					config[server_block_index].location[location_index].err_page);
		}
	}
}

/*
** allowed method check
*/
void URIParser::checkAllowedMethod() {
	char a = client.getMethod();
	int block = client.getResServerBlockIndex();
	int location = client.getResLocationIndex();
	int cgi = client.getResCgiIndex();

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
	if (cgi != -1) {
		if ((config[block].location[cgi].allowed_method & a) == GET_BIT)
			return;
		if ((config[block].location[cgi].allowed_method & a) == POST_BIT)
			return;
		if ((config[block].location[cgi].allowed_method & a) == DELETE_BIT)
			return;
		if ((config[block].location[cgi].allowed_method & a) == PUT_BIT)
			return;
		if ((config[block].location[cgi].allowed_method & a) == HEAD_BIT)
			return;
	}
	client.setStatus(405);
}

void URIParser::checkURICGI() {
	int block = client.getResServerBlockIndex();
	size_t size = config[block].location.size();

	for (size_t i = 0; i < size; ++i) {
		if  (config[block].location[i].cgi != "") {
			return setCGIPATH(block, i);
		}
	}
}

void URIParser::setCGIPATH(int const & block, int const & i) {
	if (config[block].location[i].location_uri.find(".") == std::string::npos)
		return ;
	std::string find_str = config[block].location[i].location_uri.substr(1);
	std::string uri = client.getURI();
	if (uri.find(find_str) == std::string::npos)
		return ;
	else {
		client.setResCgiIndex(i);
		client.setResponseCGIDirectory(config[block].location[i].cgi);
		client.setResponseFileDirectory("");
	}
}
