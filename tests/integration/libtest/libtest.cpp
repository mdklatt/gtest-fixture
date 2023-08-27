/**
 * Verify that the library is usable in an external CMake project by testing
 * that all headers are present and all modules are linkable.
 */
#include "gtest-fixture/environ.hpp"


/**
 * Execute the application.
 *
 * @return system exit status
 */
int main() {
    testing::fixture::EnvironFixture fixture;
    fixture.SetEnv("GTEST_FIXTURE", "1");
    return fixture.GetEnv("GTEST_FIXTURE", "0") == "1" ? 0 : 1;
}
