/**
 * Tools for sharing text fixtures.
 *
 * @file
 */
#ifndef GTEST_INFRA_SHARED_HPP
#define GTEST_INFRA_SHARED_HPP

#include <memory>
#include <stdexcept>


namespace testing::infra::shared {
    /**
     * Adaptor for sharing a fixture across multiple tests.
     *
     * @tparam Infra regular testing::infra class
     */
    template <typename Infra>
    class Shared {
    public:
        /**
         * Default constructor.
         */
        template <typename ...Args>
        explicit Shared(Args&&... args) :
            fixture{std::make_unique<Infra>(args...)} {}

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
        Infra const* operator->() const {
            if (not fixture) {
                throw std::logic_error{"invalid fixture pointer"};
            }
            return fixture.get();
        }

        /** @overload */
        Infra* operator->() {
            if (not fixture) {
                throw std::logic_error{"invalid fixture pointer"};
            }
            return fixture.get();
        }

        Shared(const Shared&) = delete;

    private:
        std::unique_ptr<Infra> fixture;
    };
}  // testing::infra::shared

#endif  // GTEST_INFRA_SHARED_HPP
