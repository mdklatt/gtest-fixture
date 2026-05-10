cmake_minimum_required(VERSION 3.16)
project(test_lib LANGUAGES CXX)

find_package(GTest REQUIRED)
if(IS_DIRECTORY ${LIBRARY_SOURCE_DIR})
    # This uses the existing project directory, so uncommitted changes may hide
    # errors that would occur in a clean checkout.
    message(STATUS "Testing library build tree at ${LIBRARY_SOURCE_DIR}")
    add_subdirectory(${LIBRARY_SOURCE_DIR} ${LIBRARY_SOURCE_DIR}/build)
else()
    # All default search locations are disabled to ensure that only the test
    # installation is found.
    message(STATUS "Testing installed library at ${path}")
#    list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/cmake")
#    message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
    find_package(GTestFixture REQUIRED)
endif()

add_executable(test_lib test_lib.cpp)
target_link_libraries(test_lib
PRIVATE
    GTestFixture::gtest-infra
    GTest::gtest
    GTest::gtest_main
)
target_compile_features(test_lib
PRIVATE
    cxx_std_17
)
