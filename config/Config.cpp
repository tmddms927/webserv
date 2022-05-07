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
    // std::ifstream file("../test.conf");
    std::ifstream file("/Users/seungoh/webserv/test.conf");

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
    if (raw[0].find(CLIENT_BODY_SIZE) != std::string::npos) {
        std::stringstream ss(raw[0].substr(strlen("client_max_body_size ")));
        ss >> global_config.client_max_body_size;
    }
    if (raw[1].find(DEFAULT_ERROR) != std::string::npos) {
        global_config.err_page = raw[1].substr(strlen(DEFAULT_ERROR));
    }
    if (raw[2].find(HOSTV) != std::string::npos)
        config[0].host = raw[2].substr(strlen(HOSTV));
    if (raw[3].find(PORT) != std::string::npos) {
        std::stringstream ss(raw[3].substr(strlen(PORT)));
        ss >> config[0].port;
        eraseCompleted();
        return;
    }
    throw GlobalConfigException();
}

void Config::validateServerVariables() {
    while (!raw.empty()) {
        std::vector<std::string>::iterator it = raw.begin();
        if (*it++ == "server") {
            config.push_back(ServerBlock::parse(raw, config[0]));
            eraseCompleted();
        }
    }
}

void Config::runParse() {
    readFile();
    setGlobalConfig();
    validateServerVariables();
}

std::vector<servers> const & Config::getConfig() const{
    return config;
}
global const & Config::getGlobal() const{
    return global_config;
}

std::ostream &operator<<(std::ostream &os, const Config &config) {
    std::cout << "Config result" << std::endl;
    std::cout << "=============================================="<< std::endl;
    for (int i = 0; i < config.config.size(); ) {
        std::cout << "--------------------------------------" << std::endl;
        std::cout << "port : " << config.config[i].port << std::endl;
        std::cout << "host : " << config.config[i].host << std::endl;
        std::cout << "location : " << config.config[i].location << std::endl;
        std::cout << "root : " << config.config[i].root << std::endl;
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
    return "Check config file rule";
}