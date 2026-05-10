/**
 * Test suite for the 'environ' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-infra/environ.hpp"
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <string>

using namespace testing::infra::environ;
using testing::Test;
using std::getenv;
using std::make_unique;
using std::string;


/**
 * Test suite for the Environ class.
 */
class EnvironTest: public Test {
protected:
    Environ environ;
};


/**
 * Test the Environ::get() method.
 */
TEST_F(EnvironTest, get) {
    EXPECT_EQ(getenv("PWD"), Environ::get("PWD"));
    EXPECT_EQ("none", Environ::get("NONE", "none"));
}


/**
 * Test the Environ::set() method.
 */
TEST_F(EnvironTest, set) {
    static const string value{"TEST"};
    for (const auto& name: {"HOME", "NEW"}) {
        // Test an existing and new variable.
        environ.set(name, value);
        EXPECT_EQ(value, Environ::get(name));
    }
}


/**
 * Test the Environ::unset() method.
 */
TEST_F(EnvironTest, unset) {
    environ.set("TESTENV", "TRUE");
    environ.unset("TESTENV");
    EXPECT_FALSE(getenv("TESTENV"));
}


/**
 * Test Environ destructor.
 */
TEST_F(EnvironTest, destruct) {
    // Ensure that all changes are rolled back when the fixture is destroyed.
    const auto home{Environ::get("HOME")};
    const auto pwd{Environ::get("PWD")};
    auto tmpenv{make_unique<Environ>()};
    tmpenv->set("TESTENV");  // added
    tmpenv->set("PWD", "NONE");  // changed
    tmpenv->unset("HOME");
    tmpenv.reset();  // invoke ~EnvFixture()
    EXPECT_FALSE(getenv("TESTENV"));
    EXPECT_EQ(pwd, getenv("PWD"));
    EXPECT_EQ(home, getenv("HOME"));
}
