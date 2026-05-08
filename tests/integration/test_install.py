""" Verify the that the installed library is usable.

"""
from pathlib import Path
from subprocess import run

import pytest


@pytest.fixture
def install(tmp_path, build, build_dir) -> dict:
    """ Configure the build to use an installed library.

    :return: CMake definitions
    """
    source_dir = Path.cwd()
    install_dir = tmp_path / "usr/local"
    defs = {
        "CMAKE_PREFIX_PATH": install_dir,
        "CMAKE_INSTALL_PREFIX": install_dir,
    }
    build(source_dir, build_dir / "lib", defs, "install")
    return defs


def test_lib(build, build_dir, install):
    """ Test the library as part of an application.

    """
    source_dir = Path(__file__).parent / "src"
    build(source_dir, build_dir, install)
    exe = build_dir / "test_lib"
    process = run([str(exe)], capture_output=True)
    assert process.returncode == 0
    return


# Make the script executable.

if __name__ == "__main__":
    raise SystemExit(pytest.main([__file__]))
