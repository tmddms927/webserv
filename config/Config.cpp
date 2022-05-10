//
// Created by gilee on 2022/05/02.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "Config.hpp"
#include "unistd.h"

Config::Config(std::string const & conf_file) : config(), raw(), global_config(), conf_file(conf_file){}

void Config::runParse() {
    readFile();
    setMainConfig();
    setServerBlock();
    validateServerBlock();
}

void Config::readFile() {
    std::ifstream file(conf_file);
    if (file.is_open()) {
        std::string buf;
        while (std::getline(file, buf)) {
            raw.push_back(buf);
        }
        file.close();
    } else {
        std::cerr << "ERROR : you need test.conf file" << std::endl;
        std::exit(1);
    }
    if (!(raw.end() - 1)->empty())
        throw VariableRuleException("End of File Must be \\n\\n");
}

void Config::setMainConfig() {
    if (raw[0].find(CLIENT_BODY_SIZE) != std::string::npos) {
        std::stringstream ss(raw[0].substr(strlen(CLIENT_BODY_SIZE)));
        ss >> global_config.client_max_body_size;
        eraseCompleted();
        return;
    }
    throw GlobalConfigException();
}

void Config::eraseCompleted() {
    while (!raw.begin()->empty())
        raw.erase(raw.begin());
    raw.erase(raw.begin());
}

void Config::setServerBlock() {
    std::vector<std::string>::iterator it = raw.begin();
    while (raw.size()) {
        if (*it == SERVERV)
            config.push_back(ServerBlock::parse(raw));
        else if (*it == "")
            raw.erase(raw.begin());
        else
            throw std::exception();
        it = raw.begin();
    }
}

void Config::validateServerBlock() {
    bool res = false;

    if (res)
        throw VariableRuleException();
}

std::ostream &operator<<(std::ostream &os, const Config &config) {
    std::cout << "Config result" << std::endl;
    std::cout << "=============================================="<< std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "client_max_body_size : " << config.global_config.client_max_body_size << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    for (int i = 0; i < config.config.size(); ) {
        std::cout << "--------------------------------------" << std::endl;
        std::cout << "server[" << i + 1 << "]" << std::endl;
        std::cout << "  port : " << config.config[i].port << std::endl;
        std::cout << "  host : " << config.config[i].host << std::endl;
        for (int j = 0; j < config.config[i].location.size(); ) {
            std::cout << "location_"<< j + 1<< "_uri : " << config.config[i].location[j].location_uri << std::endl;
            std::cout << "    location_root : " << config.config[i].location[j].location_root << std::endl;
            std::cout << "    allowed_method : " << (int)config.config[i].location[j].allowed_method << std::endl;
            std::cout << "    default_error_page : " << config.config[i].location[j].err_page << std::endl;
            std::cout << "    index : " << config.config[i].location[j].index << std::endl;
            std::cout << "    is_aster : " << config.config[i].location[j].is_aster << std::endl;
            std::cout << "    CGI : " << config.config[i].location[j].cgi << std::endl;
            j++;
        }
        std::cout << "--------------------------------------" << std::endl;
        i++;
    }
    std::cout << "==============================================";
    return os;
}

const char *Config::GlobalConfigException::what() const throw(){
    return "Check global-value rule";
}

const char *Config::VariableRuleException::what() const throw() {
    char res[100];
    std::string msg = "Check config file rule";
    ::memset(res, 0, 100);
    ::memmove(res, msg.c_str(), msg.length());
    if (!_str.empty()) {
        ::strcat(res, _str.c_str());
    }
    return res;
}

Config::VariableRuleException::VariableRuleException() {}

std::vector<servers> const & Config::getConfig() const{
    return config;
}

global const & Config::getGlobal() const{
    return global_config;
}


