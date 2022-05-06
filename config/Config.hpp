//
// Created by gilee on 2022/05/02.
//

#ifndef UNTITLED1_CONFIG_HPP
#define UNTITLED1_CONFIG_HPP

#include "ServerBlock.hpp"
#include <vector>

class Config {
private:
    std::string                 str;
    std::vector<servers>      config;
    std::vector<std::string>  raw;
    global                      global_config;
    void    readFile();
    void    serverCount();
    void    setGlobalConfig();
    void    validateServerVariables();
    void    eraseCompleted();
    class   GlobalConfigException : public std::exception {
    public:
        const char *what() const throw();
    };
    class   ParenthesisException : public std::exception {
    public:
        const char *what() const throw();
    };
    class   FirstServerBlockException : public std::exception {
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
};

#endif //UNTITLED1_CONFIG_HPP
