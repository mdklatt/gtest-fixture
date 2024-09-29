""" Define Conan configurations.

"""
from conan import ConanFile


class GTestFixtureRecipe(ConanFile):
    """ Conan recipe for the gtest-fixture library.

    """
    requires = ["gtest/1.14.0"]
    settings = ["os", "compiler", "build_type", "arch"]
    generators = ["CMakeToolchain", "CMakeDeps"]
