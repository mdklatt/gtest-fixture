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
using std::make_unique;
using std::unique_ptr;


/**
 * Test suite for the OutputFixture class.
 */
class OutputFixtureTest: public Test {
protected:
    ostringstream dest;
};


/**
 * Test OutputFixture capture with a stream.
 */
TEST_F(OutputFixtureTest, capture_stream) {
    ostringstream stream;
    OutputFixture fixture{stream, dest};
    stream << "abc";
    EXPECT_EQ("abc", dest.str());
}


/**
 * Test the OutputFixture destructor.
 */
TEST_F(OutputFixtureTest, dtor) {
    ostringstream stream;
    stream << "abc";
    auto fixture{make_unique<OutputFixture>(stream, dest)};
    stream << "def";  // captured, no output
    fixture.reset();  // call ~OutputFixture()
    stream << "xyz";
    EXPECT_EQ("abcxyz", stream.str());
}


/**
 * Test OutputFixture with passthrough.
 */
TEST_F(OutputFixtureTest, passthru) {
    ostringstream stream;
    stream << "abc";
    auto fixture{make_unique<OutputFixture>(stream, dest, true)};
    stream << "def";  // forwarded output
    fixture.reset();
    stream << "xyz";
    EXPECT_EQ("abcdefxyz", stream.str());
}


/**
 * Test OutputFixture capture of std::cout.
 */
TEST_F(OutputFixtureTest, cout) {
    OutputFixture fixture{std::cout, dest};
    std::cout << "abc";
    EXPECT_EQ("abc", dest.str());
}


/**
 * Test OutputFixture with the Shared<> adaptor.
 */
TEST_F(OutputFixtureTest, shared) {
    ostringstream stream;
    Shared<OutputFixture> fixture{stream, dest};
    stream << "abc";
    EXPECT_EQ("abc", dest.str());
    fixture.teardown();
    stream << "xyz";
    EXPECT_EQ("xyz", stream.str());
}
