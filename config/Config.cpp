//
// Created by gilee on 2022/05/02.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include "Config.hpp"
#include "unistd.h"
#include "dirent.h"

Config::Config(std::string const & conf_file) : config(), raw(), global_config(), conf_file(conf_file){}

Config::Config(const Config &src) {
    *this = src;
}

Config &Config::operator=(const Config &src) {
    this->config = src.config;
    this->raw = src.raw;
    this->global_config = src.global_config;
    this->conf_file = src.conf_file;
    return *this;
}

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
        throw VariableRuleException("end of file is not \\n\\n");
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
    ServerBlock sb;
    while (!raw.empty()) {
        if (*it == SERVERV)
            config.push_back(sb.parse(raw));
        else if (it->empty())
            raw.erase(raw.begin());
        else
            throw VariableRuleException("server block is not exist");
        it = raw.begin();
    }
}

void Config::openFile(std::string const & str) {
    std::fstream fs;
    fs.open(str);
    fs.is_open() ? fs.close() : throw VariableRuleException("file is not exist");
}

void Config::openDir(std::string const & str) {
    DIR* fs = opendir(str.c_str());
    fs ? closedir(fs) : throw VariableRuleException("directory is not exist");
}

void Config::checkResponseCode() {
    int     responses[RESPONSE_CNT] = {RESPONSE_CODES};
    for (size_t i = 0; i < config.size(); i++) {
        for (size_t j = 0; j < config[i].location.size() ; j++) {
            bool    res = false;
            if (config[i].location[j].redirect_code != -1) {
                for (int k = 0; k < RESPONSE_CNT; k++) {
                    if (config[i].location[j].redirect_code == responses[k])
                        res = true;
                }
            } else
                res = true;
            if (!res)
                throw VariableRuleException("invalid response code");
         }
    }
}

void Config::checkAllowedMethod() {
    for (size_t i = 0; i < config.size(); i++) {
        for (size_t j = 0; j < config[i].location.size() ; j++) {
            if (config[i].location[j].allowed_method == -1)
                throw VariableRuleException("you must set allowed_method");
        }
    }
}

void Config::checkHost() {
    for (size_t i = 0; i < config.size(); i++) {
        if (config[i].host.empty())
            throw VariableRuleException("host can't be NULL");
    }
}

void Config::checkPort() {
    int port = config[0].port;

    for (size_t i = 0; i < config.size(); i++) {
        if (((config[i].port < 80 || config[i].port > 87) && config[i].port < 1024) || config[i].port > 65535)
            throw VariableRuleException("invalid port number");
    }
    for (size_t i = 1; i < config.size(); i++) {
        if (port == config[i].port)
            throw VariableRuleException("not allow duplicated port");
    }
}

void Config::checkFile() {
    for (size_t i = 0; i < config.size(); i++) {
        for (size_t j = 0; j < config[i].location.size() ; j++) {
            openFile(config[i].location[j].location_root + "/" + config[i].location[j].index);
            openFile(config[i].location[j].location_root + "/" + config[i].location[j].err_page);
            if (!config[i].location[j].cgi.empty())
                openFile(config[i].location[j].cgi);
        }
    }
}

void Config::checkDirDepth() {
    for (size_t i = 0; i < config.size() ; i++) {
        for (size_t j = 0; j < config[i].location.size() ; j++) {
            int rootCnt = 0;
            std::string::iterator it = config[i].location[j].location_uri.begin();
            while (it != config[i].location[j].location_uri.end()) {
                if (*it++ == '/')
                    rootCnt++;
            }
            if (rootCnt > 2)
                throw VariableRuleException("max dir depth is 1");
        }
    }
}

void Config::checkDir() {
    for (size_t i = 0; i < config.size(); i++) {
        for (size_t j = 0; j < config[i].location.size() ; j++) {
            if (*(config[i].location[j].location_root.end() - 1) == '/')
                throw VariableRuleException("end of dir is can't be \\");
            openDir(config[i].location[j].location_root);
        }
    }
}


void Config::checkRelativePath() {
    bool res = 0;
    for (size_t i = 0; i < config.size() ; i++) {
        for (size_t j = 0; j < config[i].location.size() ; j++) {
            res |= config[i].location[j].location_root.find("..") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].location_root.find("./") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].index.find("..") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].index.find("./") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].err_page.find("..") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].err_page.find("./") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].location_uri.find("..") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].location_uri.find("./") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].cgi.find("..") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].cgi.find("./") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].redirect_uri.find("..") != std::string::npos ? 1 : 0;
            res |= config[i].location[j].redirect_uri.find("./") != std::string::npos ? 1 : 0;
        }
    }
    if (res)
        throw VariableRuleException("can't use relative path");
}

void Config::checkVariables() {
    checkDir();
    checkFile();
    checkPort();
    checkHost();
    checkDirDepth();
    checkAllowedMethod();
    checkResponseCode();
}

bool Config::hasRootLocation() {
    bool res = false;
    for (size_t i = 0; i < config.size(); i++) {
        for (size_t j = 0; j < config[i].location.size(); j++) {
            if (config[i].location[j].location_uri == "/")
                res = true;
        }
    }
    return res;
}

void Config::validateServerBlock() {
    checkVariables();
    checkRelativePath();
    if (!hasRootLocation())
        throw VariableRuleException("must have root variable");
}

std::ostream &operator<<(std::ostream &os, const Config &config) {
    std::cout << "Config result" << std::endl;
    std::cout << "=============================================="<< std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "client_max_body_size : " << config.global_config.client_max_body_size << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    for (size_t i = 0; i < config.config.size(); ) {
        std::cout << "--------------------------------------" << std::endl;
        std::cout << "server[" << i + 1 << "]" << std::endl;
        std::cout << "  port : " << config.config[i].port << std::endl;
        std::cout << "  host : " << config.config[i].host << std::endl;
        for (size_t j = 0; j < config.config[i].location.size(); ) {
            std::cout << "location_"<< j + 1<< "_uri : " << config.config[i].location[j].location_uri << std::endl;
            std::cout << "    location_root : " << config.config[i].location[j].location_root << std::endl;
            std::cout << "    allowed_method : " << (int)config.config[i].location[j].allowed_method << std::endl;
            std::cout << "    default_error_page : " << config.config[i].location[j].err_page << std::endl;
            std::cout << "    index : " << config.config[i].location[j].index << std::endl;
            std::cout << "    is_aster : " << config.config[i].location[j].is_aster << std::endl;
            std::cout << "    CGI : " << config.config[i].location[j].cgi << std::endl;
            std::cout << "    auto_index : " << config.config[i].location[j].auto_index << std::endl;
            std::cout << "    client_max_body_size : " << config.config[i].location[j].client_max_body_size << std::endl;
            std::cout << "    redirect_uri : " << config.config[i].location[j].redirect_uri << std::endl;
            std::cout << "    redirect_code : " << config.config[i].location[j].redirect_code << std::endl;
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

std::vector<servers> const & Config::getConfig() const{
    return config;
}

global const & Config::getGlobal() const{
    return global_config;
}

const char *Config::VariableRuleException::what() const _NOEXCEPT{
    return message.c_str();
}
