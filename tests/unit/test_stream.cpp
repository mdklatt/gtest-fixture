/**
 * Test suite for the 'stream' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/stream.hpp"
#include "gtest-fixture/shared.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>

using namespace testing::fixture;
using testing::Test;
using std::istringstream;
using std::ostringstream;
using std::make_unique;
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
