//
// Created by gilee on 2022/05/05.
//

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <vector>
#include <string>
#include "util.h"

class ServerBlock {
private:
    class InvalidServerBlock : std::exception {
        const char *what() const throw();
    };
    class InvalidLocationBlock : std::exception {
        const char *what() const throw();
    };
    static bool        checkVariables(servers const & tmp, servers const & src);
    static locations   parse_location(std::vector<std::string> & raw, std::vector<std::string>::iterator & it);
    static void        validMethod(char & allowed_bits, std::string const & methods);
public:
    static servers     parse(std::vector<std::string> & raw);
};


#endif //SERVERBLOCK_HPP
