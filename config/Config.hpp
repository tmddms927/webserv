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
    std::vector<servers>            config;
    std::vector<std::string>        raw;
    global                          global_config;
    std::string                     conf_file;

private:
    void                            readFile();
    void                            setMainConfig();
    void                            eraseCompleted();
    void                            setServerBlock();
    void                            checkVariables();
    void                            checkRelativePath();
    void                            checkResponseCode();
    bool                            hasRootLocation();
    void                            checkDirDepth();
    void                            checkAllowedMethod();
    void                            checkPort();
    void                            checkHost();
    void                            openFile(std::string const & str);
    void                            openDir(std::string const & str);
    void                            checkFile();
    void                            checkDir();
    void                            validateServerBlock();
private:
    class   GlobalConfigException : public std::exception {
    public:
        const char                  *what() const _NOEXCEPT;
    };
    class   VariableRuleException : public std::exception {
    private:
        std::string message;
    public:
        explicit VariableRuleException(std::string const & _m) : message("Check config file : " + _m) {}
        ~VariableRuleException() _NOEXCEPT {};
        const char *what() const _NOEXCEPT;
    };

public:
    explicit                        Config(std::string const & conf_file);
    Config(Config const & src);
    Config  &                       operator=(Config const & src);
    std::vector<servers> const &    getConfig() const;
    global const &                  getGlobal() const;
    void                            runParse();

    friend std::ostream &           operator<<(std::ostream &os, const Config &config);
};

#endif //UNTITLED1_CONFIG_HPP
