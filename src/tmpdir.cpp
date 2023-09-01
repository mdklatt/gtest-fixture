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

std::filesystem::path TmpDirFixture::run_root;  // lazy initialization

size_t TmpDirFixture::test_count{0};


TmpDirFixture::TmpDirFixture() {
    const auto test{UnitTest::GetInstance()->current_test_info()};
    test_root = std::filesystem::path{test->test_suite_name()} / test->name();
    run_path(test_root, true);
    ++test_count;
}


TmpDirFixture::~TmpDirFixture() {
    // Leave the most recent run directories intact to allow additional action
    // to be taken with test output, but remove older run directories.
    if (--test_count > 0) {
        // Run isn't complete yet.
        return;
    }
    auto run_dirs{list_dirs()};
    while (run_dirs.size() > max_dir_count) {
        // Directories are sorted by run numbers, which are assumed to be in
        // chronological order, so start removing directories at the beginning.
        const auto oldest{run_dirs.begin()};
        const auto path{oldest->second};
        remove_all(path);  // don't care if this fails
        run_dirs.erase(oldest);
    }
}


std::filesystem::path TmpDirFixture::run_path(const std::string &subdir, bool create) {
    if (run_root.empty()) {
        make_root();
    }
    const auto path{subdir.empty() ? run_root : run_root / subdir};  // no trailing slash
    if (create and not create_directories(path)) {
        throw std::runtime_error{"could not create directory " + path.string()};
    }
    return path;
}


std::filesystem::path TmpDirFixture::test_path(const std::string &subdir, bool create) const {
    const auto path{subdir.empty() ? test_root : test_root / subdir};  // no trailing slash
    return run_path(path, create);
}


void TmpDirFixture::make_root() {
    const auto root{temp_directory_path() / "gtest"};
    auto run_dirs{list_dirs()};
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
    run_root = path;
}


map<size_t, std::filesystem::path> TmpDirFixture::list_dirs() {
    map<size_t, std::filesystem::path> run_dirs;
    if (not is_directory(root_dir)) {
        return run_dirs;
    }
    static const regex run_regex{R"(^run-(\d+)$)"};  // capture run number
    smatch match;
    for (const auto& item: directory_iterator{root_dir}) {
        // Find all run directories in root.
        const auto name{item.path().filename().string()};
        if (regex_search(name, match, run_regex)) {
            const auto key{stoul(match.str(1))};
            run_dirs.emplace(key, item.path());
        }
    }
    return run_dirs;  // sorted by default
}
