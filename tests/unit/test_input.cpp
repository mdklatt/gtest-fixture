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
private:
    TmpDirFixture tmpdir;

protected:
    /**
     * Per-test setup run before each test.
     */
    InputFixtureTest() {
        ofstream stream{path};
        stream << "abc";
    }

    std::filesystem::path path{tmpdir.test_path() / "test.txt"};
};


/**
 * Test the InputFixture constructor for stream input.
 */
TEST_F(InputFixtureTest, ctor_stream) {
    istringstream stream;
    ifstream input{path};
    InputFixture fixture{stream, input};
    EXPECT_EQ('a', stream.get());
}


/**
 * Test InputFixture constructor for string input.
 */
TEST_F(InputFixtureTest, ctor_string) {
    istringstream stream;
    InputFixture fixture{stream, string{"abc"}};
    EXPECT_EQ('a', stream.get());
}


/**
 * Test InputFixture constructor for file input.
 */
TEST_F(InputFixtureTest, ctor_path) {
    istringstream stream;
    InputFixture fixture{stream, path};
    EXPECT_EQ('a', stream.get());
}


/**
 * Test the InputFixture destructor.
 */
TEST_F(InputFixtureTest, dtor) {
    istringstream stream{"xyz"};
    auto fixture{make_unique<InputFixture>(stream, path)};
    EXPECT_EQ('a', stream.get());
    fixture.reset();  // call ~InputFixture()
    EXPECT_EQ('x', stream.get());
}


/**
 * Test InputFixture capture of std::cin.
 */
TEST_F(InputFixtureTest, cin) {
    InputFixture fixture{std::cin, path};
    EXPECT_EQ('a', std::cin.get());
}


/**
 * Test InputFixture with the Shared<> adaptor.
 */
TEST_F(InputFixtureTest, shared) {
    istringstream stream{"xyz"};
    Shared<InputFixture> fixture{stream, path};
    EXPECT_EQ('a', stream.get());
    fixture.teardown();
    EXPECT_EQ('x', stream.get());
}
