""" Verify the that the library is usable as a vcpkg package.

"""
from json import load
from pathlib import Path

import pytest
from jsonschema import Draft7Validator
from referencing import Registry, Resource


@pytest.fixture
def schema(assets) -> dict:
    """ Master vcpkg schema for testing

    :return: schema JSON
    """
    # <https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json>
    return load(assets.joinpath("vcpkg.schema.json").open("rt"))


@pytest.fixture
def registry(assets, schema) -> Registry:
    """ Registry of local copies of schema dependencies.

    :return: registry containing required schema
    """
    root, _ = schema["$id"].rsplit("/", 1)
    registry = Registry()
    for path in assets.glob("vcpkg*.schema.json"):
        # Register each schema file.
        resource = Resource.from_contents(load(path.open("rt")))
        registry = registry.with_resource(f"{root}/{path.name}", resource)
    return registry


def test_config(version, schema, registry):
    """ Test the library as part of an application.

    """
    # Just a basic sanity test in lieu of installing the pacakge with `vcpkg`.
    config = load(Path("vcpkg.json").open("rt"))
    assert config["version"] == version
    validator = Draft7Validator(schema, registry=registry)
    assert validator.is_valid(config)
    return


# Make the script executable.

if __name__ == "__main__":
    raise SystemExit(pytest.main([__file__]))
