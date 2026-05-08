""" Verify the that the library is usable as a Conan package.

"""
import pytest
from pathlib import Path
from subprocess import run


@pytest.fixture(scope="module")
def requires() -> list[str]:
    """ Conan requirement specifiers.

    :return: requirements
    """
    version = Path("version.txt").read_text().strip()
    return [f"gtest-fixture/{version}", "gtest/1.14.0"]


@pytest.fixture(scope="module")
def package():
    """ Create a Conan package.

    """
    # Expects the project root to be $PWD.
    argv = ["conan", "create", str(Path.cwd())]
    process = run(argv, capture_output=True)
    if process.returncode != 0:
        raise RuntimeError(f"conan create failed: {process.returncode}")
    return


@pytest.mark.usefixtures("package")
def test_lib(build, build_dir):
    """ Test the library as part of an application.

    """
    source_dir = Path(__file__).parent
    build(source_dir, build_dir)
    exe = build_dir / "test_lib"
    process = run([str(exe)], capture_output=True)
    assert process.returncode == 0
    return


# Make the script executable.

if __name__ == "__main__":
    raise SystemExit(pytest.main([__file__]))
