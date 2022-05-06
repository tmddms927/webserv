//
// Created by gilee on 2022/05/02.
//

#ifndef UNTITLED1_CONFIG_HPP
#define UNTITLED1_CONFIG_HPP

#include "ServerBlock.hpp"
#include <vector>
#include <ostream>

class Config {
private:
    std::string                 str;
    std::vector<servers>      config;
    std::vector<std::string>  raw;
    global                      global_config;
    void    readFile();
    void    setGlobalConfig();
    void    validateServerVariables();
    void    eraseCompleted();
    class   GlobalConfigException : public std::exception {
    public:
        const char *what() const throw();
    };
    class   VariableRuleException : public std::exception {
    public:
        const char *what() const throw();
    };
public:
    Config();
    std::vector<servers> const & getConfig() const;
    global const & getGlobal() const;
    void    runParse();

    friend std::ostream &operator<<(std::ostream &os, const Config &config);
};

#endif //UNTITLED1_CONFIG_HPP
