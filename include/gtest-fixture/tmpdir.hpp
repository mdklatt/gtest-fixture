/**
 * Tools for working with temporary tests directories.
 *
 * @file
 */
#ifndef GTEST_FIXTURE_TMPDIR_HPP
#define GTEST_FIXTURE_TMPDIR_HPP

#include <filesystem>
#include <map>


namespace testing::fixture {

/**
 * Fixture for managing temporary directories.
 */
class TmpDirFixture {
public:
    /**
     * Get a tmp directory for the current run.
     *
     * Each execution of a GoogleTest executable is considered a run. A clean
     * root directory is created to hold all test directories for each run.
     * The most recent directories are left intact, so test output is available
     * until the run directory is recycled (the fixture will remove older run
     * directories, and the system will periodically purge tmp files).
     *
     * In most cases it is not necessary to call this method; instead, use
     * `test()` to get a clean directory for each test. However, this method
     * can be used to create subdirectories that can be shared by multiple
     * tests in a run.
     *
     * @param subdir optional subdirectory within the run root
     * @param create create the subdirectory if it does not exist
     * @return absolute directory path
     */
    static std::filesystem::path run_path(const std::string& subdir = "", bool create = false);

    /**
     * Default constructor.
     *
     * TmpDirFixture instances can only be created within a text context, i.e.
     * as a non-static member of a test suite or inside a TEST* function.
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
    std::filesystem::path test_path(const std::string& subdir = "", bool create = false) const;

private:
    static constexpr size_t max_dir_count{10};
    static const std::filesystem::path root_dir;
    static std::filesystem::path run_root;
    static size_t test_count;
    std::filesystem::path test_root;  // relative to run_root

    /**
     * Create the root tmp directory for this run.
     */
    static void make_root();

    /**
     * Get a listing of the existing run directories.
     *
     * @return directory paths keyed and sorted by sequence number
     */
    static std::map<size_t, std::filesystem::path> list_dirs();
};

}  // namespace


#endif  // GTEST_FIXTURE_TMPDIR_HPP
