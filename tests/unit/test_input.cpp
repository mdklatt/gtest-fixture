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


