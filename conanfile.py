from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class GTestFixtureRecipe(ConanFile):
    """

    """
    name = "gtest-fixture"
    version = "0.1.0"

    settings = "os", "compiler", "build_type", "arch"
    requires = "gtest/1.14.0"

    generators = "CMakeToolchain", "CMakeDeps"

    options = {
        "build_tests": [True, False],
    }
    default_options = {
        "build_tests": False
    }


    exports_sources = (
        "CMakeLists.txt",
        "src/*",
        "include/*",
        "cmake/*",
    )

    test_package_folder = "tests/package"

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={
            "BUILD_TESTING": self.options.build_tests
        })
        cmake.build()
        return

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        properties = {
            "cmake_file_name": "gtest-fixture",
            "cmake_target_name": "gtest-fixture::gtest-fixture",
        }
        map(self.cpp_info.set_property, properties.items())
        self.cpp_info.libs = ["gtest-fixture"]
        return

