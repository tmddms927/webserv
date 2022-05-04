//
// Created by gilee on 2022/05/02.
//
#include <gtest/gtest.h>
#include "../Config.hpp"

TEST(Config_test, readFileTest) {
    Config a;
    a.readFile();
    EXPECT_NO_THROW({
        try {
            Config e;
            e.readFile();
            e.serverCount();
            e.setGlobalConfig();
            e.validateParenthesis();
            e.validateFirstServerBlock();
            e.validateServerVariables();
        } catch (std::exception const & e) {
            throw;
        }
        });
    //a.validateServerVariables();
    //a.serverCount();
}
