""" Verify the that the library is usable as an external build tree.

"""
import pytest
from pathlib import Path
from subprocess import run


def test_lib(build, build_dir):
    """ Test the library as part of an application.

    """
    defs = {
        "LIBRARY_SOURCE_DIR": Path.cwd(),
    }
    source_dir = Path(__file__).parent
    build(source_dir, build_dir, defs)
    exe = build_dir / "test_lib"
    process = run([str(exe)], capture_output=True)
    assert process.returncode == 0
    return


# Make the script executable.

if __name__ == "__main__":
    raise SystemExit(pytest.main([__file__]))
