cmake_minimum_required(VERSION 3.15)
project(test_packagee LANGUAGES CXX)

find_package(GTestFixture REQUIRED)

add_executable(test_package test_package.cpp)

target_link_libraries(test_package
    GTestFixture::gtest-fixture
)

target_compile_features(test_package PRIVATE cxx_std_17)
