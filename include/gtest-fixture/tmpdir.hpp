/**
 * Create a temporary directory for testing.
 *
 * @file
 */
#ifndef GTEST_FIXTURE_TMPDIR_HPP
#define GTEST_FIXTURE_TMPDIR_HPP

#include <filesystem>


namespace testing::fixture {

/**
 * Provide temporary test directories for a test suite.
 */
class TmpDirFixture {
public:
    /**
     * Get a tmp directory for the current run.
     *
     * Each execution of a GoogleTest executable is considered a run. A clean
     * root directory is created to hold all test directories for each run.
     * This directory is *not* automatically deleted after the run, so it is
     * available for inspection until it is recycled (see below).
     *
     * Each run directory is numbered to guarantee uniqueness, *e.g*
     * `gtest/run-5`. The `max_count` class attribute determines the maximum
     * number of run directories that will be created. Once this limit is
     * reached, the first directory (`run-1`) is recycled, and the sequence
     * resets.
     *
     * In most cases it is not necessary to call this method; instead, use
     * `TmpTestDir()` to get a clean directory for each test. This method can
     * be used for cases where multiple tests need to share a directory, but it
     * is the caller's responsibility to avoid directory name collisions.
     *
     * @param subdir optional subdirectory within the run root
     * @param create create the subdirectory if it does not exist
     * @return absolute directory path
     */
    static std::filesystem::path TmpRootDir(const std::string& subdir = "", bool create = false);

    /**
     * Default constructor.
     */
    TmpDirFixture();

    /**
     * Get a tmp directory for the current test.
     *
     * Each test has its own tmp directory, and test directories are grouped by
     * test suite with the unique root directory for this run.
     *
     * @param subdir optional subdirectory within the test root
     * @param create create the subdirectory if it does not exist
     * @return absolute directory path
     */
    std::filesystem::path TmpTestDir(const std::string& subdir = "", bool create = false) const;

private:
    static constexpr size_t max_count{10};
    static std::filesystem::path root_dir;
    std::filesystem::path test_dir;  // relative to root_dir

    /**
     * Create the root tmp directory for this run.
     */
    static void MakeRootDir();
};

}  // namespace


#endif  // GTEST_FIXTURE_TMPDIR_HPP
