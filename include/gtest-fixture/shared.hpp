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
        template <typename ...Args>
        explicit Shared(Args... args) :
            fixture{std::make_unique<Fixture>(args...)} {}

        /**
         * Tear down the underlying fixture.
         *
         * This must be called in the `TearDown` method of the desired scope.
         */
        void teardown() {
            fixture = nullptr;
        }

        /**
         * Access the underlying fixture instance.
         *
         * @return instance pointer
         */
        Fixture const* operator->() const {
            return operator->();
        }

        /** @overload */
        Fixture* operator->() {
            if (not fixture) {
                throw std::logic_error{"invalid fixture pointer"};
            }
            return fixture.get();
        }

        Shared(const Shared&) = delete;

    private:
        std::unique_ptr<Fixture> fixture;
    };
}

#endif  // GTEST_FIXTURE_SHARED_HPP
