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
using std::getenv;
using std::make_unique;
using std::string;


/**
 * Test suite for the EnvironFixture class.
 */
class EnvironFixtureTest: public Test {
protected:
    EnvironFixture environ;
};


/**
 * Test the EnvironFixture::GetEnv() method.
 */
TEST_F(EnvironFixtureTest, get) {
    EXPECT_EQ(getenv("PWD"), EnvironFixture::get("PWD"));
    EXPECT_EQ("none", EnvironFixture::get("NONE", "none"));
}


/**
 * Test the EnvironFixture::SetEnv() method.
 */
TEST_F(EnvironFixtureTest, set) {
    static const string value{"TEST"};
    for (const auto& name: {"HOME", "NEW"}) {
        // Test an existing and new variable.
        environ.set(name, value);
        EXPECT_EQ(value, EnvironFixture::get(name));
    }
}


/**
 * Test the EnvironFixture::DeleteEnv() method.
 */
TEST_F(EnvironFixtureTest, unset) {
    environ.set("TESTENV", "TRUE");
    environ.unset("TESTENV");
    EXPECT_FALSE(getenv("TESTENV"));
}


/**
 * Test EnvironFixture destructor.
 */
TEST_F(EnvironFixtureTest, destruct) {
    // Ensure that all changes are rolled back when the fixture is destroyed.
    const auto home{EnvironFixture::get("HOME")};
    const auto pwd{EnvironFixture::get("PWD")};
    auto tmpenv{make_unique<EnvironFixture>()};
    tmpenv->set("TESTENV");  // added
    tmpenv->set("PWD", "NONE");  // changed
    tmpenv->unset("HOME");
    tmpenv.reset();  // invoke ~EnvFixture()
    EXPECT_FALSE(getenv("TESTENV"));
    EXPECT_EQ(pwd, getenv("PWD"));
    EXPECT_EQ(home, getenv("HOME"));
}
