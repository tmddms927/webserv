#include "URIParser.hpp"
#include "../../autoindex/AutoIndex.hpp"

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
	findDefaultLocationIndex();
	if (checkGoBeforeDirectory())
		return ;
	findServerBlockIndex();
	findServerLocationIndex();
	checkURICGI();
	checkAllowedMethod();
}

bool URIParser::checkGoBeforeDirectory() {
	if (client.getURI().find("/../") != std::string::npos) {
		client.setStatus(404);
		return true;
	}
	return false;
}

/*
** find default location index
*/
void URIParser::findDefaultLocationIndex() {
	size_t size = server_socket.size();
	int server_block = client.getResServerBlockIndex();

	for (size_t i = 0; i < size; ++i) {
		if (config[server_block].location[i].location_uri == "/") {
			default_location = i;
			client.setResLocationIndex(i);
			return ;
		}
	}
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
	if (findServerLocationIndex_findServerBlock2())
		return ;
	// server block location과 uri가 맞지 않은 경우
	client.setResponseFileDirectory(\
			config[client.getResServerBlockIndex()].location[client.getResLocationIndex()].location_root\
			+ config[client.getResServerBlockIndex()].location[client.getResLocationIndex()].err_page);
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
			config[client.getResServerBlockIndex()].location[client.getResLocationIndex()].location_root\
			+ "/" + config[client.getResServerBlockIndex()].location[client.getResLocationIndex()].index);
		return true;
	}
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
	std::string temp;

	for (int i = 0; i < size; ++i) {
		if (client.getResLocationIndex() == i)
			continue ;
		if (config[server_block].location[i].location_uri.length() > uri.length())
			continue ;
		temp = uri.substr(0, config[server_block].location[i].location_uri.length());
		if (temp == config[server_block].location[i].location_uri) {
			temp = uri.substr(config[server_block].location[i].location_uri.length());

			client.setResLocationIndex(i);
			if (temp == "" || temp == "/") {
				std::cout << client.getResLocationIndex() << std::endl;
				findServerLocationIndex_findServerBlock3();
				isFileAutoIndex();
			}
			else {
				client.setResponseFileDirectory(config[server_block].location[i].location_root + temp);
				isFile();
			}
			return true;
		}
	}
	client.setResponseFileDirectory(\
		config[server_block].location[client.getResLocationIndex()].location_root + uri);
	isFile();
	return true;
}

/*
** findServerLocationIndex
** 127.0.0.1/abc/ 일 경우
*/
void URIParser::findServerLocationIndex_findServerBlock3() {
	int server_block = client.getResServerBlockIndex();
	int location = client.getResLocationIndex();

	client.setResponseFileDirectory(\
		config[server_block].location[location].location_root + \
		"/" + config[server_block].location[location].index);
}

/*
** server block location - asterisk check
*/
bool URIParser::findServerLocationIndex_checkAsterisk(std::string const & str) {
	int server_block = client.getResServerBlockIndex();
	int location_index = client.getResLocationIndex();
	std::string uri;
	size_t found;

	if (config[server_block].location[client.getResLocationIndex()].is_aster == true)
		return true;
	if (config[server_block].location[location_index].is_aster != true) {
		uri = str.substr(1);
		found = uri.find("/");
		uri = uri.substr(found + 1);
		if (found != std::string::npos && uri.length() != 0) {
			client.setResLocationIndex(client.getResLocationIndex());
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

	if (stat(path.c_str(), &ss) == -1 && client.getResponseCGIDirectory() == "" &&
		client.getMethod() != PUT_BIT && client.getMethod() != POST_BIT)
		client.setStatus(404);
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

/*
**
*/
void URIParser::checkAutoIndex() {
	bool err;
	std::string body;

	if (config[client.getResServerBlockIndex()].\
		location[client.getResLocationIndex()].auto_index != 1) {
		client.setStatus(404);
		return ;
	}

	AutoIndex autoIndex(config[client.getResServerBlockIndex()].\
		location[client.getResLocationIndex()].location_root);
	err = autoIndex.makeHTML();
	if (err)
		client.setStatus(404);
	else {
		body = autoIndex.getRes().body;
		client.setResponseHeader("Server", SERVER_DEFAULT_NAME);
		client.setResponseHeader("Date", "Tue, 26 Apr 2022 10:59:45 GMT");
		client.setStatus(200);
		client.setResponseBody(body);
		client.setResponseHeader("Content-Length", ft_itoa(body.length()));
		client.setResponseLine();
	}
}

void URIParser::isFileAutoIndex() {
	std::string path = client.getResponseFileDirectory();
	int server_block_index = client.getResServerBlockIndex();
	int location_index = client.getResLocationIndex();
	struct stat ss;

	if (stat(path.c_str(), &ss) == -1 && client.getResponseCGIDirectory() == "")
		checkAutoIndex();
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
