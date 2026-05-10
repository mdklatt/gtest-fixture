# Define targets.

add_executable(test_lib
    test_environ.cpp
    test_network.cpp
    test_shared.cpp
    test_stream.cpp
    test_tmpdir.cpp
)
target_link_libraries(test_lib
PRIVATE
    GTestInfra::gtest-infra
    GTest::gtest_main
)


# Add tests to CTest suite.

include(GoogleTest)
gtest_discover_tests(test_lib WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
