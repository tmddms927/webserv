#include <iostream>
#include <fstream>
#include <string>
#include "../HTTP/HTTP.hpp"

int main(int argc, char **argv) {
	std::ifstream	testfile;
	std::string		buf;
	std::string		file;
	HTTP	http;
	if (argc != 2)
		std::cout << "usage: a.out [filename]" << std::endl;

	testfile.open(argv[1]);
	if (testfile.fail())
		std::cout << "fileopen fail" << std::endl;
	
	while (getline(testfile, buf)) {
		// std::cout << "buf : " << buf << std::endl;
		size_t found = 0;
		while ((found = buf.find("*", found)) != std::string::npos)
			buf.replace(found, 1, "\r");
		found = 0;
		while ((found = buf.find("#", found)) != std::string::npos)
			buf.replace(found, 1, "\n");
		http.reqInputBuf(buf);
		file += buf;
	}
	std::cout << "################ file ################" << std::endl;
	std::cout << file << std::endl;
	std::cout << "################ file ################" << std::endl;

	

	return 0;
}