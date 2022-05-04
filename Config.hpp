//
// Created by gilee on 2022/05/02.
//

#ifndef UNTITLED1_CONFIG_HPP
#define UNTITLED1_CONFIG_HPP

#include <map>
typedef struct  global_s {
    std::string err_page;
    int         client_max_body_size;
}               global;

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
    global                      global_config;
    void    readFile();
    void    serverCount();
    void    setGlobalConfig();
    void    validateParenthesis();
    void    validateFirstServerBlock();
    void    validateServerVariables();
public:
    Config();
    std::map<int, servers> const & getConfig() const;
    global const & getGlobal() const;
    void    runParse();
};

#endif //UNTITLED1_CONFIG_HPP
