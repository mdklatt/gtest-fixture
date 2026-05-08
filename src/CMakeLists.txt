include(CMakePackageConfigHelpers)
include(GNUInstallDirs)



# Define targets.

add_library(gtest-fixture
    environ.cpp
    network.cpp
    stream.cpp
    tmpdir.cpp
)
add_library(GTestFixture::gtest-fixture ALIAS gtest-fixture)

target_include_directories(gtest-fixture
PUBLIC
    # Interface paths are relative to the installation path, not the current
    # directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gtest-fixture
PRIVATE
    GTest::gtest
)

# Generate package config files.


configure_package_config_file(
    cmake/GTestFixtureConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/GTestFixtureConfig.cmake
INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/GTestFixture
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/GTestFixtureConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)


# Define installation rules.

install(TARGETS gtest-fixture
    EXPORT GTestFixtureTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


install(
FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GTestFixtureConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GTestFixtureConfigVersion.cmake
DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/GTestFixture
)

install(EXPORT GTestFixtureTargets
    FILE GTestFixtureTargets.cmake
    NAMESPACE GTestFixture::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GTestFixture
)


# Export targets. If an external project adds this project to its build tree
# via `add_subdirectory()`, then `find_package()` will work without
# requiring this project to be installed.

export(EXPORT GTestFixtureTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/GTestFixtureTargets.cmake
    NAMESPACE GTestFixture::
)
export(PACKAGE GTestFixture)
