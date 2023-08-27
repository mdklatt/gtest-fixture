/**
 * Create a temporary directory for testing.
 */
#ifndef GTEST_FIXTURE_TMPDIR_HPP
#define GTEST_FIXTURE_TMPDIR_HPP

#include <filesystem>


namespace testing::fixture {

class TmpDirFixture {
public:
    /**
     * Default constructor.
     */
     TmpDirFixture() = default;
};

}  // namespace


#endif  // GTEST_FIXTURE_TMPDIR_HPP
