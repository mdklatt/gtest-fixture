""" Integration tests.

This will verify that the library is usable by another CMake project as an
external build tree or an installed library.

"""
import pytest
from pathlib import Path
from subprocess import PIPE, run, STDOUT
from shlex import split
from typing import Callable


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
def conan(tmp_path_factory, build_type):
    """ Install dependencies with Conan.

    :return: path to Conan toolchain file
    """
    root = tmp_path_factory.mktemp("conan")
    conanfile = "tests/integration/conanfile.py"
    argv = ["conan", "install", conanfile, f"--output-folder={root}", "-s", f"build_type={build_type}"]
    process = run(argv, stdout=PIPE, stderr=STDOUT, text=True)
    output = process.stdout.split("\n")  # debugging
    if process.returncode != 0:
        raise RuntimeError(f"conan failed: {process.returncode}")
    toolchain = root.joinpath("build", build_type, "generators", "conan_toolchain.cmake")
    try:
        yield toolchain
    finally:
        presets = Path("tests", "integration", "CMakeUserPresets.json")
        presets.unlink()
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
        return process.returncode

    def run_build(source: Path, build: Path, defs: dict, target=None):
        """ """
        default_defs = {
            "CMAKE_BUILD_TYPE": build_type,
            "CMAKE_TOOLCHAIN_FILE": conan,
        }
        defs = default_defs | defs
        config_args = [f"-D{key}={value}" for key, value in defs.items()]
        config_args.extend(["-S", source, "-B", build])
        run_cmake(*config_args)
        build_args = ["--build", build]
        if target:
            build_args.extend(["--target", target])
        run_cmake(*build_args)

    return run_build


@pytest.fixture
def install(tmp_path, conan, build, build_dir) -> dict:
    """ Configure the build to use an installed library.

    :return: CMake definitions
    """
    source_dir = Path.cwd()
    install_dir = tmp_path / "usr/local"
    defs = {
        "BUILD_TESTING": "OFF",
        "CMAKE_PREFIX_PATH": install_dir,
        "CMAKE_INSTALL_PREFIX": install_dir,
    }
    build(source_dir, build_dir / "lib", defs, "install")
    return defs


@pytest.fixture
def source(conan) -> dict:
    """ Configure the build to the library source tree.

    :return: CMake definitions
    """
    return {
        "BUILD_TESTING": "OFF",
        "CMAKE_TOOLCHAIN_FILE": conan,
        "LIBRARY_SOURCE_DIR": Path.cwd(),
    }


@pytest.fixture(params=["install", "source"])
def app(request, conan, build, build_dir) -> Path:
    """ Build the C++ test application.

    :return: application path
    """
    defs = request.getfixturevalue(request.param)
    local_dir = Path(__file__).parent / "src"
    build(local_dir, build_dir, defs)
    return build_dir / "test_lib"


def test_lib(app):
    """ Test the library as part of an application.

    """
    process = run(split(str(app)), capture_output=True)
    assert process.returncode == 0
    return


# Make the script executable.

if __name__ == "__main__":
    raise SystemExit(pytest.main([__file__]))
