#include "gtest-fixture/tmpdir.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <deque>
#include <limits>
#include <map>
#include <regex>
#include <string>

using testing::fixture::TmpDirFixture;
using testing::UnitTest;
using std::deque;
using std::filesystem::create_directories;
using std::filesystem::directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::path;
using std::filesystem::remove_all;
using std::filesystem::temp_directory_path;
using std::map;
using std::min;
using std::max;
using std::numeric_limits;
using std::regex;
using std::regex_search;
using std::runtime_error;
using std::smatch;
using std::sort;
using std::stoul;
using std::string;
using std::to_string;


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
    // TODO: Too long, needs refactoring.
    static const regex run_regex{R"(^run-(\d+)$)"};  // capture run number
    const auto root{temp_directory_path() / "gtest"};
    if (not is_directory(root) and not create_directories(root)) {
        // Test if directory exists first because create_directories() returns
        // false if no directory was created.
        throw runtime_error{"could not create directory: " + root.string()};
    }
    smatch match;
    map<size_t, std::filesystem::path> run_dirs;
    deque<size_t> run_keys;
    for (const auto& item: directory_iterator{root}) {
        // Find all run directories.
        const auto name{item.path().filename().string()};
        if (regex_search(name, match, run_regex)) {
            const auto key{stoul(match.str(1))};
            run_dirs.emplace(key, item.path());
            run_keys.emplace_back(key);
        }
    }
    sort(run_keys.begin(), run_keys.end());
    while (run_keys.size() > max_count - 1) {
        // The system should remove tmp directories on a regular basis, but
        // clean up old directories to be nice.
        remove_all(run_dirs.at(run_keys.front()));
        run_keys.pop_front();
    }
    const size_t next_run{run_keys.empty() ? 1 : ++run_keys.back()};
    const auto path{root / ("run-" + to_string(next_run))};
    if (not create_directories(path)) {
        // FIXME: Race condition if another gtest executable else creates 'path'.
        throw runtime_error{"could not created directory: " + path.string()};
    }
    run_dir = path;
}
