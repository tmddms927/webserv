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
            e.validateParenthesis();
            e.validateFirstServerBlock();
        } catch (std::exception const & e) {
            throw;
        }
        });
    a.validateFirstServerVariables();
    a.serverCount();
}
