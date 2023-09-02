/**
 * Test suite for the 'shared' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/environ.hpp"
#include "gtest-fixture/shared.hpp"
#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>

using namespace testing::fixture;
using testing::Test;
using std::getenv;
using std::make_unique;
using std::string;


/**
 * Test suite for the Shared<> fixture adaptor.
 */
class SharedFixtureTest: public Test {
protected:
    static Shared<EnvironFixture> environ;

    /**
     * Test suite teardown called after last test.
     */
    static void TearDownTestSuite() {
        environ.teardown();
    }
};


Shared<EnvironFixture> SharedFixtureTest::environ;


/**
 * Test the Shared<>::operator->() method.
 */
TEST_F(SharedFixtureTest, arrow) {
    environ->set("SHAREDFIXTURETEST", "1");
    EXPECT_EQ(string{"1"}, getenv("SHAREDFIXTURETEST"));
}


/**
 * Test that Shared<> resources are valid across multiple tests.
 */
TEST_F(SharedFixtureTest, shared) {
    // Variable set in previous test is still valid.
    EXPECT_EQ(string{"1"}, getenv("SHAREDFIXTURETEST"));
}


/**
 * Test the Shared<>::teardown() method.
 */
TEST_F(SharedFixtureTest, teardown) {
    environ.teardown();
    EXPECT_FALSE(getenv("SHAREDFIXTURETEST"));
}
