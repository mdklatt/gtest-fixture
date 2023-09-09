/**
 * Test suite for the 'output' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/output.hpp"
#include "gtest-fixture/shared.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>

using namespace testing::fixture;
using testing::Test;
using std::ostringstream;
using std::unique_ptr;
using std::make_unique;


/**
 * Test suite for the EnvironFixture class.
 */
class OutputFixtureTest: public Test {};


/**
 * Test the OutputFixture destructor.
 */
TEST_F(OutputFixtureTest, rollback) {
    ostringstream stream;
    stream << "abc";
    auto fixture{make_unique<OutputFixture>(stream)};
    stream << "def";  // captured, no output
    fixture.reset();  // call ~OutputFixture()
    stream << "xyz";
    EXPECT_EQ("abcxyz", stream.str());
}


/**
 * Test that an OutputFixture forwards captured output.
 */
TEST_F(OutputFixtureTest, passthru) {
    ostringstream stream;
    stream << "abc";
    auto fixture{make_unique<OutputFixture>(stream, true)};
    stream << "def";  // forwarded output
    fixture.reset();
    stream << "xyz";
    EXPECT_EQ("abcdefxyz", stream.str());
}


/**
 * Test the OutputFixture::str() method;
 */
TEST_F(OutputFixtureTest, str) {
    ostringstream stream;
    OutputFixture fixture{stream};
    stream << "abc";
    EXPECT_EQ("abc", fixture.str());
}


/**
 * Test OutputFixture capture of std::cout;
 */
TEST_F(OutputFixtureTest, cout) {
    OutputFixture fixture{std::cout};
    std::cout << "abc";
    EXPECT_EQ("abc", fixture.str());
}
