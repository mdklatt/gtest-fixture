/**
 * Test suite for the 'stream' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-infra/stream.hpp"
#include "gtest-infra/shared.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>

using namespace testing::infra::stream;
using testing::infra::shared::Shared;
using testing::Test;
using std::istringstream;
using std::ostringstream;
using std::make_unique;
using std::unique_ptr;


/**
 * Test suite for the Environ class.
 */
class InputStreamTest: public Test {
protected:
    istringstream input{"abc"};
};


/**
 * Test InputStream stream capture.
 */
TEST_F(InputStreamTest, capture) {
    istringstream stream;
    InputStream fixture{stream, input};
    EXPECT_EQ('a', stream.get());
}


/**
 * Test the InputStream destructor.
 */
TEST_F(InputStreamTest, dtor) {
    istringstream stream{"xyz"};
    auto fixture{make_unique<InputStream>(stream, input)};
    EXPECT_EQ('a', stream.get());
    fixture.reset();  // call ~InputStream()
    EXPECT_EQ('x', stream.get());
}


/**
 * Test InputStream capture of std::cin.
 */
TEST_F(InputStreamTest, cin) {
    InputStream fixture{std::cin, input};
    EXPECT_EQ('a', std::cin.get());
}


/**
 * Test InputStream with the Shared<> adaptor.
 */
TEST_F(InputStreamTest, shared) {
    istringstream stream{"xyz"};
    Shared<InputStream> fixture{stream, input};
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
