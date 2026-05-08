cmake_minimum_required(VERSION 3.16)
project(test_lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(IS_DIRECTORY ${LIBRARY_SOURCE_DIR})
    # This uses the existing project directory, so uncommitted changes may hide
    # errors that would occur in a clean checkout.
    message(STATUS "Testing library build tree at ${LIBRARY_SOURCE_DIR}")
    add_subdirectory(${LIBRARY_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/lib)
else()
    # All default search locations are disabled to ensure that only the test
    # installation is found.
    message(STATUS "Testing installed library at ${path}")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/cmake")
    message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
    find_package(gtest-fixture REQUIRED)
endif()



list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/generators")
find_package(GTest REQUIRED)

add_executable(test_lib test_lib.cpp)
target_link_libraries(test_lib
PRIVATE
    GTestFixture::gtest-fixture
    GTest::gtest_main
)
