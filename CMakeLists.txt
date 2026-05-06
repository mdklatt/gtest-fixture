cmake_minimum_required(VERSION 3.23)


# Get the project version from an external file that is also used by Conan.
set(_version_file "${CMAKE_SOURCE_DIR}/version.txt")
file(READ "${_version_file}" PROJECT_VERSION)
string(STRIP "${PROJECT_VERSION}" PROJECT_VERSION)
if(PROJECT_VERSION STREQUAL "")
    message(FATAL_ERROR "${_version_file} is empty")
endif()

project(gtest-fixture
    # Semantic Versioning: <https://semver.org>
    # The build number is optional and designates a dev version.
    VERSION ${PROJECT_VERSION}
    LANGUAGES CXX
)

list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_CURRENT_BINARY_DIR}/conan)
include(DebugASanBuildType)

option(BUILD_DOCS "Build documentation" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(GTest REQUIRED)

add_subdirectory(src)

include(GNUInstallDirs)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


# Including CTest here at the project root makes `ctest` runnable from the
# build root. This is also necessary for CLion to generate a CTest Run
# Configuration.
include(CTest)
if(BUILD_TESTING)  # defined by CTest module
    add_subdirectory(tests)
endif()

if(BUILD_DOCS)
    add_subdirectory(docs)
endif()
