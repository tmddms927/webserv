#ifndef URIPARSER_HPP
#define URIPARSER_HPP

#include <iostream>
#include <vector>
#include <sys/stat.h>
#include "../../HTTP/HTTP.hpp"

class URIParser {
private:
	HTTP							& client;
	const std::vector<uintptr_t>	& server_socket;
	const std::vector<servers>		& config;

public:
	URIParser(HTTP & _client, std::vector<uintptr_t> const & _server_socket,\
				std::vector<servers> const & _config);
	~URIParser();
    void	checkReqHeader();
	void	findServerBlockIndex();
	void	findServerLocationIndex();
	bool	findServerLocationIndex_findRoot();
	bool	findServerLocationIndex_findServerBlock1();
	bool	findServerLocationIndex_findServerBlock2();
	bool	findServerLocationIndex_findServerBlock3();
	bool	findServerLocationIndex_checkAsterisk(std::string const & str);
	int		checkLocation(int const & sb, int const & size, std::string const & str);
	void	isFile();
	void	checkAllowedMethod();
	void	checkURICGI();
	void	setCGIPATH(int const & block, int const & i);
};

#endif
