#include <gtest-fixture/shared.hpp>
#include <gtest-fixture/environ.hpp>


using namespace testing::fixture;


template <typename Fixture>
void Shared<Fixture>::teardown() {
    fixture = nullptr;
}


template <typename Fixture>
Fixture const* Shared<Fixture>::operator->() const {
    return operator->();
}


template <typename Fixture>
Fixture* Shared<Fixture>::operator->() {
    if (not fixture) {
        throw std::logic_error{"invalid SharedFixture pointer"};
    }
    return fixture.get();
}


// Explicit instantiations for all allowed usages.

template class testing::fixture::Shared<EnvironFixture>;
