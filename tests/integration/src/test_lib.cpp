/**
 * Test the gtest-infra library.
 *
 * Verify that the library is usable in an external CMake project by testing
 * that all headers are present and all modules are linkable.
 */
#include "gtest-infra/environ.hpp"
#include "gtest-infra/stream.hpp"
#include "gtest-infra/tmpdir.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace testing::fixture::environ;
using namespace testing::fixture::environ;
using testing::Test;
using std::cin;
using std::cout;
using std::istringstream;
using std::getenv;
using std::ofstream;
using std::ostringstream;
using std::string;


/**
 * Test fixture for validating fixture mixin classes.
 */
class FixtureTest: public Test {
protected:
    testing::fixture::environ::EnvironFixture environ;
    testing::fixture::tmpdir::TmpDirFixture tmpdir;
    istringstream stdin;
    ostringstream stdout;

private:
    testing::fixture::stream::InputFixture input{cin, stdin};
    testing::fixture::stream::OutputFixture output{cout, stdout};
};


/**
 * Test the EvironFixture class.
 */
TEST_F(FixtureTest, environ) {
    environ.set("FIXTURETEST", "1");
    EXPECT_EQ("1", string{getenv("FIXTURETEST")});
}


/**
 * Test the InputFixture class.
 */
TEST_F(FixtureTest, input) {
    stdin.str("TEST");
    string str;
    cin >> str;
    EXPECT_EQ(stdin.str(), str);
}


/**
 * Test the OutputFixture class.
 */
TEST_F(FixtureTest, output) {
    cout << "TEST";
    EXPECT_EQ(stdout.str(), "TEST");
}


/**
 * TEst the TmpDirFixture class.
 */
TEST_F(FixtureTest, tmpdir) {
    ofstream stream{tmpdir.test_path() / "test.txt"};
    EXPECT_TRUE(stream);
}
