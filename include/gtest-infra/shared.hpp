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
     * <https://google.github.io/googletest/advanced.html#sharing-resources-between-tests-in-the-same-test-suite>
     * <https://google.github.io/googletest/advanced.html#global-set-up-and-tear-down>
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
            target{std::make_unique<Infra>(args...)} {}

        /**
         * Tear down the underlying fixture.
         *
         * This must be called in the `TearDown` method of the desired scope.
         */
        void teardown() {
            target = nullptr;  // forces target destructor to be called
        }

        /**
         * Access the underlying fixture instance.
         *
         * @return instance pointer
         */
        Infra const* operator->() const {
            if (not target) {
                // This can happen if teardown() was called.
                throw std::logic_error{"instance is null"};
            }
            return target.get();
        }

        /** @overload */
        Infra* operator->() {
            if (not target) {
                // This can happen if teardown() was called.
                throw std::logic_error{"instance is null"};
            }
            return target.get();
        }

        Shared(const Shared&) = delete;

    private:
        std::unique_ptr<Infra> target;
    };
}  // testing::infra::shared

#endif  // GTEST_INFRA_SHARED_HPP
