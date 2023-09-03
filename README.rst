#############
gtest-fixture
#############

|c++17|
|cmake|
|license|
|tests|


Test fixture mixins to add capabilities to `GoogleTest`_ tests.


=====
Usage
=====

GoogleTest provides three levels of scoping for test isolation: per test, per
test suite, and per application. Fixtures are used to share resources within
a particular scope; a fixture rolls back any changes it made to the environment
when it goes out of scope.

In most situations, fixtures should be given test scope by using them as a
non-static member of a ``Test`` class or inside a ``TEST*`` function.

.. code-block::

    class FixtureTest: public testing::Test {
    protected:
        EnvironFixture environ;
    };

    TEST_F(FixtureTest, env_true) {
        environ.set("TESTENV", "1");
        EXPECT_EQ("1", environ.get("TESTENV", "0"));
    }

    TEST_F(FixtureTest, env_false) {
        EXPECT_EQ("0", environ.get("TESTENV", "0"));
    }


Sometimes, it is necessary to `share resources across a test suite`_. This is
done by wrapping a fixture in the ``Shared<>`` adaptor and making it a static
member of a ``Test`` class. Clean up is done in the ``TearDownTestSuite``
method. `Global resource sharing`_ is similar.

.. code-block::

    class SharedFixtureTest: public testing::Test {
    protected:
        static Shared<EnvironFixture>> environ;

        static void TearDownTestSuite() {
            environ.teardown();
        }
    };

    TEST_F(SharedFixtureTest, env1) {
        environ->set("SHARED", "1");
        EXPECT_EQ("1", environ->get("TESTENV", "0"));
    }

    TEST_F(SharedFixtureTest, env2) {
        // Relies on value set in previous test.
        EXPECT_EQ("1", environ->get("TESTENV", "0"));
    }



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

A ``TmpDirFixture`` can only be instantiated at test scope, so it should not be
used with the ``Shared<>`` adaptor. However, the static ``run_path()`` method
can be used to create a shared directory for multiple tests across one or more
test suites.


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
.. _Global resource sharing: https://google.github.io/googletest/advanced.html#global-set-up-and-tear-down
.. _GoogleTest: http://google.github.io/googletest/
.. _MIT License: http://choosealicense.com/licenses/mit
.. _share resources across a test suite: https://google.github.io/googletest/advanced.html#sharing-resources-between-tests-in-the-same-test-suite
