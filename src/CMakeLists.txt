# Define targets.

add_library(${name}
        environ.cpp
)
add_library(googletest-fixture::googletest-fixture ALIAS googletest-fixture)

target_include_directories(${name}
PUBLIC
    # Interface paths are relative to the installation path, not the current
    # directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


# Generate package config files.

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/googletest-fixture-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-fixture-config.cmake
INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/googletest-fixture
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-fixture-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)


# Define installation rules.

include(GNUInstallDirs)

install(TARGETS ${name}
    EXPORT googletest-fixture-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


install(
FILES
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-fixture-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-fixture-config-version.cmake
DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/googletest-fixture
)

install(EXPORT googletest-fixture-targets
    FILE googletest-fixture-targets.cmake
    NAMESPACE googletest-fixture::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/googletest-fixture
)


# Export targets. If an external project adds this project to its build tree
# via `add_subdirectory()`, then `find_package()` will work without
# requiring this project to be installed.

export(EXPORT googletest-fixture-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/googletest-fixture-targets.cmake
    NAMESPACE googletest-fixture::
)
export(PACKAGE googletest-fixture)
