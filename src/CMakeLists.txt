include(CMakePackageConfigHelpers)
include(GNUInstallDirs)



# Define targets.

add_library(gtest-infra
    environ.cpp
    network.cpp
    stream.cpp
    tmpdir.cpp
)
add_library(GTestInfra::gtest-infra ALIAS gtest-infra)

target_include_directories(gtest-infra
PUBLIC
    # Interface paths are relative to the installation path, not the current
    # directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(gtest-infra
PRIVATE
    GTest::gtest
)

# Generate package config files.


configure_package_config_file(
    cmake/GTestInfraConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/GTestInfraConfig.cmake
INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/GTestInfra
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/GTestInfraConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)


# Define installation rules.

install(TARGETS gtest-infra
    EXPORT GTestInfraTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


install(
FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GTestInfraConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GTestInfraConfigVersion.cmake
DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/GTestInfra
)

install(EXPORT GTestInfraTargets
    FILE GTestInfraTargets.cmake
    NAMESPACE GTestInfra::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GTestInfra
)


# Export targets. If an external project adds this project to its build tree
# via `add_subdirectory()`, then `find_package()` will work without
# requiring this project to be installed.

export(EXPORT GTestInfraTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/GTestInfraTargets.cmake
    NAMESPACE GTestInfra::
)
export(PACKAGE GTestInfra)
