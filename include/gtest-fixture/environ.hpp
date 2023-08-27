/**
 * Tools for managing the test application environment.
 *
 * @file
 */
#ifndef GTEST_FIXTURE_ENVIRON_HPP
#define GTEST_FIXTURE_ENVIRON_HPP

#include <map>
#include <memory>
#include <string>


namespace testing::fixture {
    /**
     * Test fixture for managing environment variables.
     */
    class EnvironFixture {
    public:
        /**
         * Default constructor.
         */
        EnvironFixture() = default;

        /**
         * Roll back changes to the environment.
         */
        ~EnvironFixture();

        /**
         * Get the value of an environment variable.
         *
         * This is not thread-safe.
         *
         * @param name
         * @param fallback
         * @return
         */
        static std::string GetEnv(const std::string& name, const std::string& fallback = "");

        /**
         * Set the value of an environment variable.
         *
         * If the variable already exists it will be modified. Note that
         * setting the value to a blank string does not remove the variable
         * from the environment.
         *
         * This is not thread-safe.
         *
         * @param name variable name
         * @param value variable value
         */
        void SetEnv(const std::string& name, const std::string& value = "");

        /**
         * Delete a test variable from the environment.
         *
         * Only variables set by this fixture via setenv() can be deleted,
         * otherwise false is returned.
         *
         * This is not thread-safe.
         *
         * @param name variable name
         */
        void DeleteEnv(const std::string& name);

        EnvironFixture(const EnvironFixture&) = delete;

    private:
        static std::map<std::string, std::unique_ptr<char>> global;
        std::map<std::string, std::unique_ptr<char>> local;

        /**
         * Create an environment variable string.
         *
         * @param name variable name
         * @param value variable value
         * @return string pointer (caller assumes ownership)
         */
        static char* EnvStr(const std::string& name, const std::string& value = "");
    };

}  // namespace testing::fixture

#endif  // GTEST_FIXTURE_ENVIRON_HPP
