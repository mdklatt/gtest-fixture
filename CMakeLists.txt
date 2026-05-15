cmake_minimum_required(VERSION 3.23)

# Get the project version from an external file that is also used by Conan.
# Setting `CMAKE_CONFIGURE_DEPENDS` on the version file flags the configure
# step for rerun when the version is updated.
set(_version_file "${CMAKE_CURRENT_SOURCE_DIR}/version.txt")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_version_file}")
file(READ "${_version_file}" PROJECT_VERSION)
string(STRIP "${PROJECT_VERSION}" PROJECT_VERSION)
if(PROJECT_VERSION STREQUAL "")
    message(FATAL_ERROR "${_version_file} is empty")
endif()
message(STATUS "Setting PROJECT_VERSION=${PROJECT_VERSION} from ${_version_file}")
project(GTestInfra
    # Semantic Versioning: <https://semver.org>
    # The build number is optional and designates a dev version.
    VERSION ${PROJECT_VERSION}
    LANGUAGES CXX
)

list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)
include(DebugASanBuildType)

option(BUILD_DOCS "Build documentation" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(GTest REQUIRED)

add_subdirectory(src)

include(GNUInstallDirs)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Generate the vcpkg config file. This file is flagged for update when the
# version file is modified.
set(_vcpkg_file "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json")
message(STATUS "Generating ${_vcpkg_file}")
configure_file("${_vcpkg_file}.in" "${_vcpkg_file}" @ONLY)


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
