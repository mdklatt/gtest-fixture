/**
 * Create a temporary directory for testing.
 *
 * @file
 */
#ifndef GTEST_FIXTURE_TMPDIR_HPP
#define GTEST_FIXTURE_TMPDIR_HPP

#include <filesystem>
#include <map>


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
     * The most recent directories are left intact, so test output is available
     * until the run directory is recycled (note that the system is free to
     * remove tmp directories at any time; this is commonly done on reboot).
     *
     * In most cases it is not necessary to call this method; instead, use
     * `TmpTestDir()` to get a clean directory for each test. This method can
     * be used to create directories that can be shared be multiple tests in
     * a single run, but it is the caller's responsibility to avoid name
     * collisions.
     *
     * @param subdir optional subdirectory within the run root
     * @param create create the subdirectory if it does not exist
     * @return absolute directory path
     */
    static std::filesystem::path TmpRunDir(const std::string& subdir = "", bool create = false);

    /**
     * Default constructor.
     */
    TmpDirFixture();

    /**
     * Destructor.
     */
    ~TmpDirFixture();

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
    static constexpr size_t max_dir_count{10};
    static const std::filesystem::path root_dir;
    static std::filesystem::path run_dir;
    static size_t test_count;
    std::filesystem::path test_dir;  // relative to run_dir

    /**
     * Create the root tmp directory for this run.
     */
    static void MakeRunDir();

    /**
     * Get a listing of the existing run directories.
     *
     * @return directory paths keyed and sorted by sequence number
     */
    static std::map<size_t, std::filesystem::path> GetRunDirs();
};

}  // namespace


#endif  // GTEST_FIXTURE_TMPDIR_HPP
