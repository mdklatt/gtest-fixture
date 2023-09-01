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
class TmpDirFixtureTest: public Test {
protected:
    TmpDirFixture tmpdir;
};


/**
 * Test the TmpDirFixture::run_path() method.
 */
TEST_F(TmpDirFixtureTest, run_path) {
    const auto path{TmpDirFixture::run_path()};
    EXPECT_NE(string::npos, path.string().find("gtest/run"));
}


/**
 * Test the TmpDirFixture::run_path() method for a subdirectory.
 */
TEST_F(TmpDirFixtureTest, run_path_subdir) {
    const string subdir{"xyz"};
    const auto path{TmpDirFixture::run_path(subdir)};
    EXPECT_EQ(subdir, path.filename().string());
    EXPECT_FALSE(is_directory(path));
}


/**
 * Test the TmpDirFixture::run_path() method for a new subdirectory.
 */
TEST_F(TmpDirFixtureTest, run_path_create) {
    const string subdir{"xyz"};
    const auto path{TmpDirFixture::run_path(subdir, true)};
    EXPECT_EQ(subdir, path.filename().string());
    EXPECT_TRUE(is_directory(path));
}


/**
 * Test the TmpDirFixture::test_path() method.
 */
TEST_F(TmpDirFixtureTest, test_path) {
    const auto abs_path{tmpdir.test_path()};
    const auto rel_path{relative(abs_path, TmpDirFixture::run_path())};
    EXPECT_EQ("TmpDirFixtureTest/test_path", rel_path.string());
    EXPECT_TRUE(is_directory(abs_path));
}


/**
 * Test the TmpDirFixture::test_path() method for a subdirectory.
 */
TEST_F(TmpDirFixtureTest, test_path_subdir) {
    const auto abs_path{tmpdir.test_path("xyz")};
    const auto rel_path{relative(abs_path, TmpDirFixture::run_path())};
    EXPECT_EQ("TmpDirFixtureTest/test_path_subdir/xyz", rel_path.string());
    EXPECT_FALSE(is_directory(abs_path));
}


/**
 * Test the TmpDirFixture::test_path() method for a new subdirectory.
 */
TEST_F(TmpDirFixtureTest, test_path_create) {
    const auto abs_path{tmpdir.test_path("xyz", true)};
    const auto rel_path{relative(abs_path, TmpDirFixture::run_path())};
    EXPECT_EQ("TmpDirFixtureTest/test_path_create/xyz", rel_path.string());
    EXPECT_TRUE(is_directory(abs_path));
}
