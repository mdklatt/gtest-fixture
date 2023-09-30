cmake_minimum_required(VERSION 3.16)

project(gtest-fixture
    # Semantic Versioning: <https://semver.org>
    VERSION 0.1.0.21  # add build number to designate a dev version
    LANGUAGES CXX
)

list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR}/conan)
include(DebugASanBuildType)

option(BUILD_DOCS "Build documentation" OFF)

set(name ${PROJECT_NAME})
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
