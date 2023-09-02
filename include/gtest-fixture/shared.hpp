/**
 * Tools for sharing text fixtures.
 *
 * @file
 */
#ifndef GTEST_FIXTURE_SHARED_HPP
#define GTEST_FIXTURE_SHARED_HPP

#include <memory>
#include <stdexcept>


namespace testing::fixture {
    /**
     * Adaptor for sharing a fixture across multiple tests.
     *
     * @tparam Fixture regular testing::fixture class
     */
    template <typename Fixture>
    class Shared {
    public:
        /**
         * Default constructor.
         */
        Shared() = default;

        /**
         * Tear down the underlying fixture.
         *
         * This must be called in the `TearDown` method of the desired scope.
         */
        void teardown();

        /**
         * Access the underlying fixture instance.
         *
         * @return instance pointer
         */
        Fixture const* operator->() const;

        /** @overload */
        Fixture* operator->();

        Shared(const Shared&) = delete;

    private:
        std::unique_ptr<Fixture> fixture{std::make_unique<Fixture>()};
    };
}

#endif  // GTEST_FIXTURE_SHARED_HPP
