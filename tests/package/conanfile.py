""" Conanfile for 'gtest-fixture' package testing.

"""
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class GTestFixtureTestPackage(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    test_type = "explicit"

    def requirements(self):
        """ Define local build requirements.

        """
        self.requires(self.tested_reference_str)
        self.requires("gtest/1.14.0")
        return

    def layout(self):
        cmake_layout(self)
        return

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if not self.conf.get("tools.build:skip_test", default=False):
            self.run(
                self.cpp.build.bindirs[0] + "/test_lib",
                env="conanrun"
            )
        return
