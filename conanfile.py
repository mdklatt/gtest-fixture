""" Define Conan configurations.

"""
from conan import ConanFile
from pathlib import Path


class GTestFixtureRecipe(ConanFile):
    """ Conan recipe for the gtest-fixture library.

    """
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        """ Define requirements.

        """
        self.requires("gtest/1.14.0")
        return

    def layout(self):
        """ Define artifact folders.

        """
        root = Path("build")
        if self.settings.get_safe("compiler") == "msvc":
            # MSVC is a mult-config generator.
            self.folders.generators = root / "conan"
        else:
            build = str(self.settings.build_type)
            self.folders.generators = root / build / "conan"
        return
