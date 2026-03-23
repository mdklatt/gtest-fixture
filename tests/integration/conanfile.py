from conan import ConanFile
from conan.tools.cmake import cmake_layout


class GTestFixtureTest(ConanFile):
    """

    """
    settings = "os", "arch", "compiler", "build_type"

    requires = [
        "gtest/1.14.0",
    ]

    generators = [
        "CMakeDeps",
        "CMakeToolchain",
    ]

    conf = {
        #"tools.cmake.cmaketoolchain:user_presets": "",
    }

    def layout(self):
        """

        :return:
        """
        cmake_layout(self)
        return
