//
// Created by gilee on 2022/05/02.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "Config.hpp"
#include "unistd.h"

Config::Config(std::string const & conf_file) : str(), config(1), raw(), global_config(), conf_file(conf_file){}

void Config::readFile() {
    std::ifstream file(conf_file);
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


void Config::setMainConfig() {
    setRootDir();
    if (raw[0].find(CLIENT_BODY_SIZE) != std::string::npos) {
        std::stringstream ss(raw[0].substr(strlen("client_max_body_size ")));
        ss >> global_config.client_max_body_size;
    }
    if (raw[1].find(DEFAULT_ERROR) != std::string::npos)
        global_config.err_page = raw[1].substr(strlen(DEFAULT_ERROR));
    if (raw[2].find(INDEXV) != std::string::npos)
        global_config.index = raw[2].substr(strlen(INDEXV));
    if (raw[3].find(ALLOWED_METHODV) != std::string::npos)
        validMethod(raw[3].substr(strlen(ALLOWED_METHODV)));
    if (raw[4].find(HOSTV) != std::string::npos)
        config[0].host = raw[4].substr(strlen(HOSTV));
    if (raw[5].find(PORT) != std::string::npos) {
        std::stringstream ss(raw[5].substr(strlen(PORT)));
        ss >> config[0].port;
        eraseCompleted();
        return;
    }
    throw GlobalConfigException();
}

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

void Config::validMethod(std::string const & methods) {
    std::string tmp = methods;
    if (tmp.find(CONF_GET) != std::string::npos) {
        global_config.allowed_method |= GET_BIT;
        tmp.replace(tmp.find(CONF_GET), strlen(CONF_GET), "");
    }
    if (tmp.find(CONF_POST) != std::string::npos) {
        global_config.allowed_method |= POST_BIT;
        tmp.replace(tmp.find(CONF_POST), strlen(CONF_POST), "");
    }
    if (tmp.find(CONF_DELETE) != std::string::npos) {
        global_config.allowed_method |= DELETE_BIT;
        tmp.replace(tmp.find(CONF_DELETE), strlen(CONF_DELETE), "");
    }
    if (tmp.find(CONF_PUT) != std::string::npos) {
        global_config.allowed_method |= PUT_BIT;
        tmp.replace(tmp.find(CONF_PUT), strlen(CONF_PUT), "");
    }
    if (tmp.find(CONF_HEAD) != std::string::npos) {
        global_config.allowed_method |= HEAD_BIT;
        tmp.replace(tmp.find(CONF_HEAD), strlen(CONF_HEAD), "");
    }
    if (!tmp.empty())
        throw VariableRuleException();

}

void Config::setRootDir() {
    char buf[1024];
    std::memset(buf, 0, 1024);
    getcwd(buf, 1024);
    config[0].root = buf;
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
    setMainConfig();
    isExist();
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


const char *Config::GlobalConfigException::what() const throw(){
    return "Check global-value rule";
}

const char *Config::VariableRuleException::what() const throw() {
    return "Check config file rule";
}