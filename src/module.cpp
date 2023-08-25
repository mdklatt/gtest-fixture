/**
 * Implementation of the sample library module.
 */
#include "googletest-fixture/module.hpp"

using testing::fixture::SampleClass;


int SampleClass::add(int x) const {
    return testing::fixture::add(num, x);
}


int testing::fixture::add(int x, int y) {
    return x + y;
}
