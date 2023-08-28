#############
gtest-fixture
#############

|c++17|
|cmake|
|license|
|tests|


Test fixture mixins to add capabilities to `GoogleTest`_ test suites.


=====
Usage
=====

In most cases, the easiest approach is to inherit directly from the desired
fixture:

.. code-block::

    class UnitTest: public testing::Test, protected testing::fixture::EnvironFixture {
    protected:
        TestSuite() {
            SetEnv("TEST_ENV", "FOO");
        }
    }

    TEST_F(SestSuite, env) {
        EXPECT_EQ("FOO", GetEnv("TEST_ENV");
    }


If more control over the fixture lifetime is needed, use composition instead
of inheritance.


Fixtures
--------

``EnvironFixture``

Modify environment variables for testing. Changes will be rolled back when the
fixture is destroyed. Note that changes to the environment are *not*
thread-safe.


``TmpDirFixture``

Provide tmp directories for testing. A numbered directory will be created for
each test run, and every test within that run will have its own dedicated
subdirectory. Run directories are *not* removed on exit and will be
available for inspection until the run directory is recycled.


===========
Development
===========

Create the development environment:

.. code-block::

    $ make dev


Build the project:

.. code-block::

    $ make build


Run tests:

.. code-block::

    $ make test


Build documentation:

.. code-block::

    $ make docs


.. |c++17| image:: https://img.shields.io/static/v1?label=c%2B%2B&message=17&color=informational
   :alt: C++17
.. |cmake| image:: https://img.shields.io/static/v1?label=cmake&message=3.16&color=informational
   :alt: CMake 3.16
.. |license| image:: https://img.shields.io/github/license/mdklatt/gtest-fixture
   :alt: MIT License
   :target: `MIT License`_
.. |tests| image:: https://github.com/mdklatt/gtest-fixture/actions/workflows/test.yml/badge.svg
   :alt: CI Test
   :target: `GitHub Actions`_

.. _GitHub Actions: https://github.com/mdklatt/gtest-fixture/actions/workflows/test.yml
.. _GoogleTest: http://google.github.io/googletest/
.. _MIT License: http://choosealicense.com/licenses/mit
