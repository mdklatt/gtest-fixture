""" Conanfile for the 'gtest-infra' project.

"""
from pathlib import Path

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class ProjectRecipe(ConanFile):
    """ Conan consumer/producer recipe for this project.

    This integrates with CMake for managing dependencies and building the
    project. It is also used to create and test Conan package for downstream
    consumers.
    """
    name = "gtest-infra"

    generators = "CMakeToolchain", "CMakeDeps"
    settings = "os", "compiler", "build_type", "arch"

    options = {
        "build_tests": (True, False),
    }

    default_options = {
        "build_tests": False
    }

    exports_sources = [
        "version.txt",
        "CMakeLists.txt",
        "vcpkg.json.in",
        "src/*",
        "include/*",
        "cmake/*",
    ]

    #test_package_folder = "tests/package"

    def set_version(self):
        """ Set the project version from a file.

        This file is also used by CMakeLists.txt, so this must be a
        '<major>.<minor>.<patch>[.<build>]' version, where the build number is
        optional and designates a dev version.

        """
        path = Path(self.recipe_folder, "version.txt")
        self.version = path.read_text().strip()
        return

    def requirements(self):
        """ Define project requirements.

        """
        private = {
            # Equivalent to CMake `link_libraries(PRIVATE ...)`. Requirements
            # that use this are not propagated to consumers.
            "transitive_headers": False,
            "transitive_libs": False
        }
        self.requires("gtest/[>=1.14.0 <1.19.0]", **private)
        return

    def layout(self):
        """ Define the project layout expected by the build tool.

        """
        cmake_layout(self)
        return

    def build(self):
        """ Build the project's artifacts when creating a package.

        """
        cmake = CMake(self)
        cmake.configure(variables={
            "BUILD_TESTING": self.options.build_tests
        })
        cmake.build()
        return

    def package(self):
        """ Create a package from the project's artifacts.

        """
        cmake = CMake(self)
        cmake.install()
        return

    def package_info(self):
        """ Define metadata required by package consumers.

        """
        properties = {
            "cmake_file_name": "GTestInfra",
            "cmake_target_name": "GTestInfra::gtest-infra",
        }
        for name, value in properties.items():
            self.cpp_info.set_property(name, value)
        self.cpp_info.libs = ["gtest-infra"]
        return
