#include "gtest-fixture/tmpdir.hpp"
#include <gtest/gtest.h>
#include <iterator>
#include <regex>
#include <string>

using testing::fixture::TmpDirFixture;
using testing::UnitTest;
using std::filesystem::create_directories;
using std::filesystem::directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::path;
using std::filesystem::remove_all;
using std::filesystem::temp_directory_path;
using std::map;
using std::prev;
using std::regex;
using std::regex_search;
using std::runtime_error;
using std::smatch;
using std::stoul;
using std::string;
using std::to_string;


const std::filesystem::path TmpDirFixture::root_dir{temp_directory_path() / "gtest"};

std::filesystem::path TmpDirFixture::run_dir;  // lazy initialization


TmpDirFixture::TmpDirFixture() {
    const auto test{UnitTest::GetInstance()->current_test_info()};
    test_dir = std::filesystem::path{test->test_suite_name()} / test->name();
    TmpRunDir(test_dir, true);
}


std::filesystem::path TmpDirFixture::TmpRunDir(const std::string &subdir, bool create) {
    if (run_dir.empty()) {
        MakeRunDir();
    }
    const auto path{subdir.empty() ? run_dir : run_dir / subdir};  // no trailing slash
    if (create and not create_directories(path)) {
        throw std::runtime_error{"could not create directory " + path.string()};
    }
    return path;
}


std::filesystem::path TmpDirFixture::TmpTestDir(const string& subdir, bool create) const {
    const auto path{subdir.empty() ?  test_dir : test_dir / subdir};  // no trailing slash
    return TmpRunDir(path, create);
}


void TmpDirFixture::MakeRunDir() {
    const auto root{temp_directory_path() / "gtest"};
    auto run_dirs{GetRunDirs()};
    const auto last_run{run_dirs.empty() ? 0 : prev(run_dirs.end())->first};
    static const size_t max_tries{3};
    std::filesystem::path path;
    for (auto next_run{last_run + 1}; next_run <= (last_run + max_tries); ++next_run) {
        // Allow for multiple attempts in case there's a race condition when
        // creating the new directory.
        // TODO: Would it better to just use a random run ID?
        path = root / string{"run-" + to_string(next_run)};
        if (create_directories(path)) {
            break;
        }
    }
    if (not is_directory(path)) {
        throw runtime_error{"could not create run directory in " + root.string()};
    }
    run_dir = path;
    CleanRunDirs();
}


map<size_t, std::filesystem::path> TmpDirFixture::GetRunDirs() {
    map < size_t, std::filesystem::path > run_dirs;
    if (not is_directory(root_dir)) {
        return run_dirs;
    }
    static const regex run_regex{R"(^run-(\d+)$)"};  // capture run number
    smatch match;
    for (const auto& item: directory_iterator{root_dir}) {
        // Find all run directories in 'root'.
        const auto name{item.path().filename().string()};
        if (regex_search(name, match, run_regex)) {
            const auto key{stoul(match.str(1))};
            run_dirs.emplace(key, item.path());
        }
    }
    return run_dirs;  // sorted by default
}


void TmpDirFixture::CleanRunDirs() {
    // The system should clean tmp directories on a regular basis, but clean up
    // old run directories here just to be nice.
    auto run_dirs{GetRunDirs()};
    while (run_dirs.size() > max_count) {
        // Run directories are sorted by run number, which is assumed to be
        // the same as chronological order, so start at the beginning.
        const auto oldest{run_dirs.begin()};
        const auto path{oldest->second};
        if (remove_all(path) == 0) {
            throw runtime_error{"could not remove run directory " + path.string()};
        }
        run_dirs.erase(oldest);
    }
}
