//
// Created by gilee on 2022/05/02.
//

#ifndef UNTITLED1_CONFIG_HPP
#define UNTITLED1_CONFIG_HPP

#include "ServerBlock.hpp"
#include <vector>
#include <ostream>

/* todo
 * default host --> global block
 * default port --> server block 1
 * server block host --> 없으면 global block에서 가져온다
 * server block port --> 없으면 global block에서 가져온다
 * default error page, index, allowed_method --> global => server로 옮기기
 * location 여러개 처리하기
 * uri --> *있는거 없는거 플래그 만들기
 */

class Config {
private:
    std::vector<servers>        config;
    std::vector<std::string>    raw;
    global                      global_config;
    std::string                 conf_file;
private:
    void    readFile();
    void    setMainConfig();
    void    validateServerVariables();
    void    eraseCompleted();
    /*
    void    isExist();
    void    validMethod(std::string const & methods);
    void    setRootDir();
     */
    class   GlobalConfigException : public std::exception {
    public:
        const char *what() const throw();
    };
    class   VariableRuleException : public std::exception {
    public:
        const char *what() const throw();
    };
public:
    Config(std::string const & conf_file);
    std::vector<servers> const & getConfig() const;
    global const & getGlobal() const;
    void    runParse();

    friend std::ostream &operator<<(std::ostream &os, const Config &config);
};

#endif //UNTITLED1_CONFIG_HPP
