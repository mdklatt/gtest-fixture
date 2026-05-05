include(CMakePackageConfigHelpers)
include(GNUInstallDirs)



# Define targets.

add_library(gtest-fixture
    environ.cpp
    network.cpp
    stream.cpp
    tmpdir.cpp
)
add_library(gtest-fixture::gtest-fixture ALIAS gtest-fixture)

target_include_directories(gtest-fixture
PUBLIC
    # Interface paths are relative to the installation path, not the current
    # directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
#PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(gtest-fixture
PRIVATE
    GTest::gtest
)

# Generate package config files.


configure_package_config_file(
    cmake/gtest-fixture-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/gtest-fixture-config.cmake
INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/gtest-fixture
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/gtest-fixture-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)


# Define installation rules.

install(TARGETS gtest-fixture
    EXPORT gtest-fixture-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


install(
FILES
    ${CMAKE_CURRENT_BINARY_DIR}/gtest-fixture-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/gtest-fixture-config-version.cmake
DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/gtest-fixture
)

install(EXPORT gtest-fixture-targets
    FILE gtest-fixture-targets.cmake
    NAMESPACE gtest-fixture::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gtest-fixture
)


# Export targets. If an external project adds this project to its build tree
# via `add_subdirectory()`, then `find_package()` will work without
# requiring this project to be installed.

export(EXPORT gtest-fixture-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/gtest-fixture-targets.cmake
    NAMESPACE gtest-fixture::
)
export(PACKAGE gtest-fixture)
