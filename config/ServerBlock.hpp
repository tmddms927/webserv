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
    static bool checkVariables(servers const & tmp, servers const & src);
public:
    static servers parse(std::vector<std::string> & raw, servers const & conf);
};


#endif //SERVERBLOCK_HPP
