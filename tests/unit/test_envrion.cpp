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
class EnvironFixtureTest: public Test, protected EnvironFixture {};


/**
 * Test the EnvironFixture::GetEnv() method.
 */
TEST_F(EnvironFixtureTest, GetEnv) {
    EXPECT_EQ(getenv("PWD"), GetEnv("PWD"));
    EXPECT_EQ("none", GetEnv("NONE", "none"));
}


/**
 * Test the EnvironFixture::SetEnv() method.
 */
TEST_F(EnvironFixtureTest, SetEnv) {
    static const string value{"TEST"};
    for (const auto& name: {"HOME", "NEW"}) {
        // Test an existing and new variable.
        SetEnv(name, value);
        EXPECT_EQ(value, GetEnv(name));
    }
}


/**
 * Test the EnvironFixture::DeleteEnv() method.
 */
TEST_F(EnvironFixtureTest, DeleteEnv) {
    SetEnv("TESTENV", "TRUE");
    DeleteEnv("TESTENV");
    EXPECT_FALSE(getenv("TESTENV"));
}


/**
 * Test EnvironFixture teardown.
 */
TEST_F(EnvironFixtureTest, teardown) {
    // Ensure that all changes are rolled back when the fixture is destroyed.
    const string home{GetEnv("HOME")};  // TODO: defined for GitHub Actions?
    const string pwd{GetEnv("PWD")};
    auto tmpenv{make_unique<EnvironFixture>()};
    tmpenv->SetEnv("TESTENV");  // added
    tmpenv->SetEnv("PWD", "NONE");  // changed
    tmpenv->DeleteEnv("HOME");
    tmpenv.reset();  // invoke ~EnvFixture()
    EXPECT_FALSE(getenv("TESTENV"));
    EXPECT_EQ(pwd, getenv("PWD"));
    EXPECT_EQ(home, getenv("HOME"));
}
