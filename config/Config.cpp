//
// Created by gilee on 2022/05/02.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "Config.hpp"

Config::Config() : str(), config(1), raw(), global_config(){

}

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
        std::cerr << "ERROR : you need test.conf file" << std::endl;
        std::exit(1);
    }
    if (!(raw.end() - 1)->empty())
        throw VariableRuleException();
}


void Config::eraseCompleted() {
    while (!raw.begin()->empty())
        raw.erase(raw.begin());
    raw.erase(raw.begin());
}


void Config::setGlobalConfig() {
    if (raw[0].find("client_max_body_size ") != std::string::npos) {
        std::stringstream ss(raw[0].substr(raw[0].find("client_max_body_size ") + strlen("client_max_body_size ")));
        ss >> global_config.client_max_body_size;
    }
    if (raw[1].find("default_error_page ") != std::string::npos) {
        global_config.err_page = raw[1].substr(raw[1].find("default_error_page ") + strlen("default_error_page "));
    }
    if (raw[2].find("host ") != std::string::npos)
        config[0].host = raw[2].substr(strlen("host "));
    if (raw[3].find("port ") != std::string::npos) {
        std::stringstream ss(raw[3].substr(strlen("port ")));
        ss >> config[0].port;
        eraseCompleted();
        return;
    }
    throw GlobalConfigException();
}

/*
void Config::serverCount() {
    int i = 0;
    int j = 0;
    while (i < raw.size()) {
        if (raw[i].find("server") != std::string::npos)
            j++;
        i++;
    }
    config.resize(j + 1);
}
*/
void Config::validateServerVariables() {
    while (!raw.empty()) {
        std::vector<std::string>::iterator it = raw.begin();
        if (*it++ == "server") {
            config.push_back(ServerBlock::parse(raw, config[0]));
            eraseCompleted();
        }
    }
    for (int i = 0; i < config.size(); ) {
        std::cout << "port " << config[i].port << std::endl;
        std::cout << "host " << config[i].host << std::endl;
        std::cout << "location " << config[i].location << std::endl;
        std::cout << "root " << config[i].root << std::endl;
        i++;
    }
}

void Config::runParse() {
    readFile();
    //serverCount();
    setGlobalConfig();
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
/*
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
*/