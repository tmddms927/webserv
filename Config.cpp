//
// Created by gilee on 2022/05/02.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "Config.hpp"

Config::Config() : str(), config(), raw(), global_config(){}

void Config::readFile() {
    std::ifstream file("../test.conf");

    if (file.is_open()) {
        std::string buf;
        while (std::getline(file, buf)) {
            str += buf + '\n';
            raw.push_back(buf);
        }
        file.close();
    } else {
        std::cerr << "config Error" << std::endl;
        std::exit(1);
    }
}

void Config::setGlobalConfig() {
    if (raw[0].find("client_max_body_size ") != std::string::npos) {
        std::stringstream ss(raw[0].substr(raw[0].find("client_max_body_size ") + strlen("client_max_body_size ")));
        ss >> global_config.client_max_body_size;
    }
    if (raw[1].find("default_error_page ") != std::string::npos) {
        global_config.err_page = raw[1].substr(raw[1].find("default_error_page ") + strlen("default_error_page "));
        raw.erase(raw.begin());
        raw.erase(raw.begin());
        std::cout << global_config.err_page << std::endl;
        std::cout << global_config.client_max_body_size << std::endl;
        return;
    }
    throw GlobalConfigException();
}

void Config::validateParenthesis() {
    std::string::iterator it = str.begin();
    int left = 0;
    int right = 0;
    while (it != str.end()){
        if (*it == '{')
            ++left;
        if (*it == '}')
            ++right;
        it++;
    }
    if (left != right)
        throw ParenthesisException();
}

void Config::serverCount() {
    int i = 0;
    int j = 0;
    while (i < raw.size()) {
        if (raw[i] == "http {")
            j++;
        if (raw[i] == "    server {")
            j++;
        i++;
    }
    config.resize(j);
}

void Config::validateFirstServerBlock() {
    if (raw[0] != "http {")
        throw FirstServerBlockException();
}

void Config::validateServerVariables() {
    int i = 0;
    int j = 0;
    while (i < raw.size()) {
        if (raw[i] == "" || raw[i].find('}') != std::string::npos) {
            i++;
            continue;
        }
        if (raw[i].find("http {") || raw[i].find("server {")) {
            if (raw[++i].find("host ") == std::string::npos)
                throw VariableRuleException();
            config[j].host = raw[i].substr(raw[i].find("host ") + strlen("host "));
            if (raw[++i].find("port ") == std::string::npos)
                throw VariableRuleException();
            std::stringstream ss(raw[i].substr(raw[i].find("port ") + strlen("port ")));
            ss >> config[j].port;
            if (j != 0) {
                if (raw[++i].find("location ") == std::string::npos)
                    throw VariableRuleException();
                config[j].location = raw[i].substr(raw[i].find("location ") + strlen("location "));
                config[j].location.erase(config[j].location.length() - 2, config[j].location.length());
            }
        }
        i++;
        j++;
    }
    for (i = 0; i < config.size(); i++) {
        std::cout << config[i].host << std::endl;
        std::cout << config[i].port << std::endl;
        std::cout << config[i].location << std::endl;
    }
}

void Config::runParse() {
    readFile();
    serverCount();
    setGlobalConfig();
    validateParenthesis();
    validateFirstServerBlock();
    validateServerVariables();
}

std::vector<servers> const & Config::getConfig() const{
    return config;
}
global const & Config::getGlobal() const{
    return global_config;
}

//todo root location --> 초기값을 어케하지~
/*
 *      todo parsing 규칙
 * 1. 최상위 서버블록의 이름은 http ✔
 * 2. 최상위 서버블록 이외의 서버블록은 하위 서버블록 없음! ✔
 * 3. 하위 서버블록의 구분은 tabspace! ✔
 * 4. parsing 시작시에 가장먼저 괄호 체크 ✔
 * 5. body size limit 추가 ✔
 * 6. default error page 추가 ✔
*/
const char *Config::GlobalConfigException::what() const throw(){
    return "Check global-value rule";
}

const char *Config::ParenthesisException::what() const throw() {
    return "Check parenthesis";
}

const char *Config::FirstServerBlockException::what() const throw() {
    return "First server block is \"http\"";
}

const char *Config::VariableRuleException::what() const throw() {
    return "Check config file rule";
}
