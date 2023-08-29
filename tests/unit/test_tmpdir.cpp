/**
 * Test suite for the 'tmpdir' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/tmpdir.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace testing::fixture;
using testing::Test;
using std::filesystem::is_directory;
using std::string;


/**
 * Test suite for the TmpDirFixture class.
 */
class TmpDirFixtureTest: public Test, protected TmpDirFixture {};


/**
 * Test the TmpDirFixture::TmpRunDir() method.
 */
TEST_F(TmpDirFixtureTest, TmpRunDir) {
    const auto path{TmpRunDir()};
    EXPECT_NE(string::npos, path.string().find("gtest/run"));
}


/**
 * Test the TmpDirFixture::TmpTestDir() method.
 */
TEST_F(TmpDirFixtureTest, TmpTestDir) {
    const auto path{TmpTestDir()};
    EXPECT_NE(string::npos, path.string().find("TmpTestDir"));
    EXPECT_TRUE(is_directory(path));
}


/**
 * Test the TmpDirFixture::TmpTestDir() method with a subdirectory.
 */
TEST_F(TmpDirFixtureTest, TmpTestDir_subdir) {
    const auto path{TmpTestDir("xyz")};
    EXPECT_NE(string::npos, path.string().find("TmpTestDir_subdir/xyz"));
    EXPECT_FALSE(is_directory(path));
}


/**
 * Test the TmpDirFixture::TmpTestDir() method for a new subdirectory.
 */
TEST_F(TmpDirFixtureTest, TmpTestDir_create) {
    const auto path{TmpTestDir("xyz", true)};
    EXPECT_NE(string::npos, path.string().find("TmpTestDir_create/xyz"));
    EXPECT_TRUE(is_directory(path));
}
