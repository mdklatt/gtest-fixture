""" Verify the that the library is usable as a vcpkg package.

"""
from json import load
from pathlib import Path

import pytest
from jsonschema import validate


def test_config(assets, version):
    """ Test the library as part of an application.

    """
    # Just a basic sanity test in lieu of installing the pacakge with `vcpkg`.
    config = load(Path("vcpkg.json").open("rt"))
    assert config["version"] == version
    schema = load(assets.joinpath("vcpkg.schema.json").open("rt"))
    assert validate(instance=config, schema=schema) is None
    return


# Make the script executable.

if __name__ == "__main__":
    raise SystemExit(pytest.main([__file__]))
