""" Global test configuration for this subdirectory.

"""
from pathlib import Path
from subprocess import PIPE, run, STDOUT
from typing import Callable

import pytest


@pytest.fixture(scope="session")
def assets() -> Path:
    """ Test assets directory

    :return: assets path
    """
    return Path(__file__).parent / "assets"


@pytest.fixture(scope="session")
def version() -> str:
    """ Get the current project version.

    :return: version string
    """
    return Path("version.txt").read_text().strip()


@pytest.fixture
def build_dir(tmp_path) -> Path:
    """ Return a temporary build directory.

    :return: build directory path
    """
    return tmp_path / "build"


@pytest.fixture(params=["Debug", "Release"], scope="module")
def build_type(request) -> str:
    """ CMake build type.

    :return: build type name
    """
    return request.param


@pytest.fixture(scope="module")
def requires() -> list[str]:
    """ Conan requirement specifiers.

    :return: requirements
    """
    return ["gtest/1.18.0"]  # test against the latest version


@pytest.fixture(scope="module")
def recipe(tmp_path_factory, requires) -> Path:
    """ Path to the Conan recipe file.

    :return: Conanfile path
    """
    generators = ["CMakeToolchain", "CMakeDeps"]
    config = [
        "[generators]",
        "\n".join(generators),
        "[requires]",
        "\n".join(requires),
    ]
    recipe = tmp_path_factory.mktemp("conan") / "conanfile.txt"
    recipe.write_text("\n".join(config) + "\n")
    return recipe


@pytest.fixture(scope="module")
def conan(tmp_path_factory, build_type, recipe):
    """ Install dependencies with Conan.

    The generated toolchain file will point to the installed packages.

    :return: path to Conan toolchain file
    """
    root = tmp_path_factory.mktemp("conan")
    argv = [
        "conan", "install", str(recipe),
        "--build=missing",
        f"--output-folder={root}",
        f"--settings=build_type={build_type}",
    ]
    process = run(argv, stdout=PIPE, stderr=STDOUT, text=True)
    output = process.stdout.split("\n")  # debugging
    if process.returncode != 0:
        raise RuntimeError(f"conan install failed: {process.returncode}")
    toolchain = root / "conan_toolchain.cmake"
    yield toolchain
    return


@pytest.fixture
def build(build_type, conan) -> Callable:
    """ Build a CMake project.

    """
    def run_cmake(*args):
        """ Execute `cmake` command. """
        argv = ["cmake"] + list(map(str, args))
        process = run(argv, stdout=PIPE, stderr=STDOUT, text=True)
        output = process.stdout.split("\n")  # debugging
        if process.returncode != 0:
            raise RuntimeError(f"cmake failed: {process.returncode}")
        return

    def run_build(source: Path, build: Path, defs=None, target=None):
        """ Execute a CMake build. """
        default_defs = {
            "CMAKE_BUILD_TYPE": build_type,
            "CMAKE_TOOLCHAIN_FILE": conan,
            "BUILD_TESTING": "OFF",
        }
        defs = default_defs | (defs or {})
        config_args = [f"-D{key}={value}" for key, value in defs.items()]
        config_args.extend(["-S", source, "-B", build])
        run_cmake(*config_args)
        build_args = ["--build", build]
        if target:
            build_args.extend(["--target", target])
        return run_cmake(*build_args)

    return run_build
