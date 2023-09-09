# Define targets.

add_executable(test_${name}
    test_envrion.cpp
    test_output.cpp
    test_shared.cpp
    test_tmpdir.cpp
)
target_link_libraries(test_${name}
PRIVATE
    ${name}
    GTest::gtest_main
)


# Add tests to CTest suite.

include(GoogleTest)
gtest_discover_tests(test_${name} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
