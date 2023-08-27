/**
 * Test suite for the 'tmpdir' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/tmpdir.hpp"
#include <gtest/gtest.h>
#include <filesystem>

using namespace testing::fixture;
using testing::Test;


/**
 * Test suite for the EnvFixture class.
 */
class EnvironFixtureTest: public Test, protected TmpDirFixture {};
