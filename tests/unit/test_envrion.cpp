/**
 * Test suite for the 'environ' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "googletest-fixture/environ.hpp"
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
 * Test suite for the EnvFixture class.
 */
class EnvFixtureTest: public Test {
protected:
    EnvironFixture environ;
};


/**
 * Test the EnvFixture::getenv() method.
 */
TEST_F(EnvFixtureTest, getenv) {
    // FIXME: This might fail in non-login environments, e.g. GitHub Actions
    EXPECT_EQ(getenv("PWD"), environ.getenv("PWD"));
    EXPECT_EQ("none", environ.getenv("NONE", "none"));
}


/**
 * Test the EnvFixture::setenv() method.
 */
TEST_F(EnvFixtureTest, setenv) {
    static const string value{"TEST"};
    for (const auto& name: {"HOME", "NEW"}) {
        // Test an existing and new variable.
        environ.setenv(name, value);
        EXPECT_EQ(value, getenv(name));
    }
}


/**
 * Test the EnvFixture::delenv() method.
 */
TEST_F(EnvFixtureTest, delenv) {
    environ.setenv("TESTENV", "TRUE");
    environ.delenv("TESTENV");
    EXPECT_FALSE(getenv("TESTENV"));
}


/**
 * Test the EnvFixture destructor.
 */
TEST_F(EnvFixtureTest, dtor) {
    // Ensure that all changes are rolled back when the fixture is destroyed.
    const string home{getenv("HOME")};  // TODO: defined for GitHub Actions?
    const string pwd{getenv("PWD")};
    auto tmpenv{make_unique<EnvironFixture>()};
    tmpenv->setenv("TESTENV");  // added
    tmpenv->setenv("PWD", "NONE");  // changed
    tmpenv->delenv("HOME");
    tmpenv.reset();  // invoke ~EnvFixture()
    EXPECT_FALSE(getenv("TESTENV"));
    EXPECT_EQ(pwd, getenv("PWD"));
    EXPECT_EQ(home, getenv("HOME"));
}
