//
// Created by gilee on 2022/05/02.
//
#include <fstream>
#include <iostream>

#include "Config.hpp"

Config::Config() : str(){}

void Config::readFile() {
    std::ifstream file("../test.conf");

    if (file.is_open()) {
        std::string buf;
        int i = 0;
        while (std::getline(file, buf)) {
            str += buf + '\n';
            raw[i++] = buf;
        }
        std::cout << str << std::endl;
        file.close();
    } else {
        std::cerr << "config Error" << std::endl;
        std::exit(1);
    }
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
        throw std::exception();
}

void Config::validateFirstServerBlock() {
    if (raw[0] != "http {")
        throw std::exception();
}

void Config::validateFirstServerName() {
    if (raw[1].find("    server_names ") == std::string::npos)
        throw std::exception();
    //todo  server 구조에 저장
}

void Config::runParse() {
    readFile();
    validateParenthesis();
}

//todo • Choose the port and host of each ’server’. --> 우리 소켓 여러개 열어야함!
/*
 *      todo parsing 규칙
 * 1. 최상위 서버블록의 이름은 http;
 * 2. 최상위 서버블록 이외의 서버블록은 하위 서버블록 없음!
 * 3. 하위 서버블록의 구분은 tabspace!
 * 4. parsing 시작시에 가장먼저 괄호 체크
*/