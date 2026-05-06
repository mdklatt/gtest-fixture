cmake_minimum_required(VERSION 3.15)
project(test_gtest_fixture LANGUAGES CXX)

find_package(gtest-fixture REQUIRED)

add_executable(test_lib test_lib.cpp)

target_link_libraries(test_lib
    gtest-fixture::gtest-fixture
)

target_compile_features(test_lib PRIVATE cxx_std_17)
