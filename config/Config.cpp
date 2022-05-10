//
// Created by gilee on 2022/05/02.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "Config.hpp"
#include "unistd.h"

Config::Config(std::string const & conf_file) : config(), raw(), global_config(), conf_file(conf_file){}

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
        throw VariableRuleException();
}


void Config::eraseCompleted() {
    while (!raw.begin()->empty())
        raw.erase(raw.begin());
    raw.erase(raw.begin());
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
/*
void Config::isExist() {
    std::vector<std::string> files;
    files.push_back(global_config.index);
    files.push_back(global_config.err_page);

    for (int i = 0; i < files.size(); i++) {
        std::ifstream file(files[i]);
        if (file.is_open()) {
            file.close();
            continue;
        }
        throw GlobalConfigException();
    }
}
*/

void Config::validateServerVariables() {
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

void Config::runParse() {
    readFile();
    setMainConfig();
    validateServerVariables();
}

std::vector<servers> const & Config::getConfig() const{
    return config;
}
global const & Config::getGlobal() const{
    return global_config;
}
/*
std::ostream &operator<<(std::ostream &os, const Config &config) {
    std::cout << "Config result" << std::endl;
    std::cout << "=============================================="<< std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "client_max_body_size : " << config.global_config.client_max_body_size << std::endl;
    std::cout << "default_error_page : " << config.global_config.err_page << std::endl;
    std::cout << "index : " << config.global_config.index << std::endl;
    std::cout << "allowed_method : " << (int)config.global_config.allowed_method << std::endl;
    std::cout << "--------------------------------------" << std::endl;
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
*/

const char *Config::GlobalConfigException::what() const throw(){
    return "Check global-value rule";
}

const char *Config::VariableRuleException::what() const throw() {
    return "Check config file rule";
}