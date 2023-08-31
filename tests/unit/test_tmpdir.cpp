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
using std::filesystem::relative;
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
 * Test the TmpDirFixture::TmpRunDir() method for a subdirectory.
 */
TEST_F(TmpDirFixtureTest, TmpRunDir_subdir) {
    const string subdir{"xyz"};
    const auto path{TmpRunDir(subdir)};
    EXPECT_EQ(subdir, path.filename().string());
    EXPECT_FALSE(is_directory(path));
}


/**
 * Test the TmpDirFixture::TmpRunDir() method for a new subdirectory.
 */
TEST_F(TmpDirFixtureTest, TmpRunDir_create) {
    const string subdir{"xyz"};
    const auto path{TmpRunDir(subdir, true)};
    EXPECT_EQ(subdir, path.filename().string());
    EXPECT_TRUE(is_directory(path));
}


/**
 * Test the TmpDirFixture::TmpTestDir() method.
 */
TEST_F(TmpDirFixtureTest, TmpTestDir) {
    const auto abs_path{TmpTestDir()};
    const auto rel_path{relative(abs_path, TmpRunDir())};
    EXPECT_EQ("TmpDirFixtureTest/TmpTestDir", rel_path.string());
    EXPECT_TRUE(is_directory(abs_path));
}


/**
 * Test the TmpDirFixture::TmpTestDir() method for a subdirectory.
 */
TEST_F(TmpDirFixtureTest, TmpTestDir_subdir) {
    const auto abs_path{TmpTestDir("xyz")};
    const auto rel_path{relative(abs_path, TmpRunDir())};
    EXPECT_EQ("TmpDirFixtureTest/TmpTestDir_subdir/xyz", rel_path.string());
    EXPECT_FALSE(is_directory(abs_path));
}


/**
 * Test the TmpDirFixture::TmpTestDir() method for a new subdirectory.
 */
TEST_F(TmpDirFixtureTest, TmpTestDir_create) {
    const auto abs_path{TmpTestDir("xyz", true)};
    const auto rel_path{relative(abs_path, TmpRunDir())};
    EXPECT_EQ("TmpDirFixtureTest/TmpTestDir_create/xyz", rel_path.string());
    EXPECT_TRUE(is_directory(abs_path));
}
