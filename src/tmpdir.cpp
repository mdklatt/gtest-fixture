#include "gtest-fixture/tmpdir.hpp"
#include <gtest/gtest.h>
#include <string>

using testing::fixture::TmpDirFixture;
using testing::UnitTest;
using std::filesystem::create_directories;
using std::filesystem::is_directory;
using std::filesystem::path;
using std::filesystem::remove_all;
using std::filesystem::temp_directory_path;
using std::string;
using std::to_string;


std::filesystem::path TmpDirFixture::root_dir;  // lazy initialization


TmpDirFixture::TmpDirFixture() {
    const auto test{UnitTest::GetInstance()->current_test_info()};
    test_dir = std::filesystem::path{test->test_suite_name()} / test->name();
    TmpRootDir(test_dir, true);
}


std::filesystem::path TmpDirFixture::TmpRootDir(const string& subdir, bool create) {
    if (root_dir.empty()) {
        MakeRootDir();
    }
    const auto path{subdir.empty() ?  root_dir : root_dir / subdir};  // no trailing slash
    if (create and not create_directories(path)) {
        throw std::runtime_error{"could not create directory " + path.string()};
    }
    return path;
}


std::filesystem::path TmpDirFixture::TmpTestDir(const string& subdir, bool create) const {
    const auto path{subdir.empty() ?  test_dir : test_dir / subdir};  // no trailing slash
    return TmpRootDir(path, create);
}


void TmpDirFixture::MakeRootDir() {
    const auto root{temp_directory_path() / "gtest"};
    std::filesystem::path path;
    size_t count{0};
    do {
        // Find an available directory. If the max count is exceeded, remove
        // the (presumably) oldest directory and start over.
        // FIXME: This will always reset to 1 once wraparound is reached.
        // TODO: Need to cycle to 2, 3, etc (compare directory times?)
        path = root / ("run-" + to_string(++count));
        if (count > max_count) {
            path = root / "run-1";
            remove_all(path);
            break;
        }
    } while (is_directory(path));
    root_dir = path;
    if (not create_directories(root_dir)) {
        throw std::runtime_error{"could not create tmpdir " + path.string()};
    }
}
