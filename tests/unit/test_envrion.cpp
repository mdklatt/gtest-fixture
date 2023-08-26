/**
 * Test suite for the 'environ' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/environ.hpp"
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <string>

using namespace testing::fixture;
using testing::Test;
using std::make_unique;
using std::string;


/**
 * Test suite for the EnvFixture class.
 */
class EnvironFixtureTest: public Test, protected EnvironFixture {};


/**
 * Test the EnvironFixture::getenv() method.
 */
TEST_F(EnvironFixtureTest, getenv) {
    EXPECT_EQ(std::getenv("PWD"), getenv("PWD"));
    EXPECT_EQ("none", getenv("NONE", "none"));
}


/**
 * Test the EnvironFixture::setenv() method.
 */
TEST_F(EnvironFixtureTest, setenv) {
    static const string value{"TEST"};
    for (const auto& name: {"HOME", "NEW"}) {
        // Test an existing and new variable.
        setenv(name, value);
        EXPECT_EQ(value, getenv(name));
    }
}


/**
 * Test the EnvironFixture::delenv() method.
 */
TEST_F(EnvironFixtureTest, delenv) {
    setenv("TESTENV", "TRUE");
    delenv("TESTENV");
    EXPECT_FALSE(std::getenv("TESTENV"));
}


/**
 * Test EnvironFixture teardown.
 */
TEST_F(EnvironFixtureTest, teardown) {
    // Ensure that all changes are rolled back when the fixture is destroyed.
    const string home{getenv("HOME")};  // TODO: defined for GitHub Actions?
    const string pwd{getenv("PWD")};
    auto tmpenv{make_unique<EnvironFixture>()};
    tmpenv->setenv("TESTENV");  // added
    tmpenv->setenv("PWD", "NONE");  // changed
    tmpenv->delenv("HOME");
    tmpenv.reset();  // invoke ~EnvFixture()
    EXPECT_FALSE(std::getenv("TESTENV"));
    EXPECT_EQ(pwd, std::getenv("PWD"));
    EXPECT_EQ(home, std::getenv("HOME"));
}
