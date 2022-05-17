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
    locations   parse_location(std::vector<std::string> & raw, std::vector<std::string>::iterator & it);
    void        validMethod(char & allowed_bits, std::string const & methods);
public:
    class InvalidServerBlock : public std::exception {
    private:
        std::string message;
    public:
        explicit InvalidServerBlock(std::string const & _m) : message("Invalid Server Block : " + _m) {};
        ~InvalidServerBlock() _NOEXCEPT{};
        const char *what() const _NOEXCEPT;
    };
    class InvalidLocationBlock : public std::exception {
    private:
        std::string message;
    public:
        explicit InvalidLocationBlock(std::string const & _m) : message("Invalid Location Block : " + _m) {};
        ~InvalidLocationBlock()_NOEXCEPT{};
        const char *what() const _NOEXCEPT;
    };
    servers     parse(std::vector<std::string> & raw);
};


#endif //SERVERBLOCK_HPP
