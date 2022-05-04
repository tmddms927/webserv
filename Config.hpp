//
// Created by gilee on 2022/05/02.
//

#ifndef UNTITLED1_CONFIG_HPP
#define UNTITLED1_CONFIG_HPP

#include <map>

typedef struct  servers_s {
    std::string host;
    std::string location;
    int         port;
}               servers;

class Config {
private:
    std::string                 str;
    std::map<int, servers>      config;
    std::map<int, std::string>  raw;
public:
    Config();
    void    readFile();
    void    serverCount();
    void    validateParenthesis();
    void    validateFirstServerBlock();
    void    validateFirstServerVariables();
    void    runParse();
};

#endif //UNTITLED1_CONFIG_HPP
