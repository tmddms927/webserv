//
// Created by gilee on 2022/05/02.
//
#include <gtest/gtest.h>
#include "../autoindex/AutoIndex.hpp"

TEST(AutoIndex_test, valid_case) {
    std::cout << "-----------------------------------------------start" << std::endl;
    ResponseMessage tmp;
    Config config("../test.conf");
    config.runParse();
    AutoIndex index(config.getConfig()[0].location[0].location_root);
    if (!index.makeHTML()) {
        tmp = index.getRes();
        std::cout << tmp.body << std::endl;
    }
    std::cout << "-----------------------------------------------end" << std::endl;
}

TEST(AutoIndex_test, invalid_case) {
    std::cout << "-----------------------------------------------invalid_start" << std::endl;
    ResponseMessage tmp;
    Config config("../test.conf");
    config.runParse();
    AutoIndex index("asf");
    if (!index.makeHTML()) {
        tmp = index.getRes();
        std::cout << tmp.body << std::endl;
    }
    std::cout << "-----------------------------------------------invalid_end" << std::endl;
}
