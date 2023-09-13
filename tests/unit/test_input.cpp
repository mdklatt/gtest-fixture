/**
 * Test suite for the 'input' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/input.hpp"
#include "gtest-fixture/shared.hpp"
#include "gtest-fixture/tmpdir.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

using namespace testing::fixture;
using testing::Test;
using std::ifstream;
using std::istringstream;
using std::make_unique;
using std::ofstream;
using std::string;
using std::unique_ptr;


/**
 * Test suite for the EnvironFixture class.
 */
class InputFixtureTest: public Test {
protected:
    istringstream input{"abc"};
};


/**
 * Test InputFixture stream capture.
 */
TEST_F(InputFixtureTest, capture) {
    istringstream stream;
    InputFixture fixture{stream, input};
    EXPECT_EQ('a', stream.get());
}


/**
 * Test the InputFixture destructor.
 */
TEST_F(InputFixtureTest, dtor) {
    istringstream stream{"xyz"};
    auto fixture{make_unique<InputFixture>(stream, input)};
    EXPECT_EQ('a', stream.get());
    fixture.reset();  // call ~InputFixture()
    EXPECT_EQ('x', stream.get());
}


/**
 * Test InputFixture capture of std::cin.
 */
TEST_F(InputFixtureTest, cin) {
    InputFixture fixture{std::cin, input};
    EXPECT_EQ('a', std::cin.get());
}


/**
 * Test InputFixture with the Shared<> adaptor.
 */
TEST_F(InputFixtureTest, shared) {
    istringstream stream{"xyz"};
    Shared<InputFixture> fixture{stream, input};
    EXPECT_EQ('a', stream.get());
    fixture.teardown();
    EXPECT_EQ('x', stream.get());
}
