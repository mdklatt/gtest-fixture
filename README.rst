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

The fixtures are designed to release resources and roll back any system changes
they make when they go out of scope. In most situations, they should be given
test scope by using them as non-static members of a ``Test`` class or inside
a ``TEST*`` function. This will isolate each test in its own environment.

.. code-block::

    class EnvironTest: public testing::Test {
    protected:
        testing::fixture::EnvironFixture environ;
    };

    TEST_F(EnvironTest, env_true) {
        environ.set("TESTENV", "1");
        EXPECT_EQ("1", environ.get("TESTENV", "0"));
    }

    TEST_F(EnvironTest, env_false) {
        EXPECT_EQ("0", environ.get("TESTENV", "0"));
    }


Sometimes, it is necessary to `share resources across tests`_. To share a
fixture across multiple tests of the same test suite, declare it as a static
pointer member of the ``Test`` class and use ``SetUpTestSuite()`` and
``TearDownTestSuite()`` to control its scope with dynamic allocation.


.. code-block::

    class SharedEnvironTest: public testing::Test {
    protected:
        static std::unique_ptr<testing::fixture::EnvironFixture> environ;

        static void SetUpTestSuite() {
            environ = make_unique<EnvironFixture>();
        }

        static void TearDownTestSuite() {
            environ = nullptr;  // destroy fixture
        }
    };

    TEST_F(SharedEnvironTest, shared1) {
        environ->set("SHARED", "1");
        EXPECT_EQ("1", environ->get("TESTENV", "0"));
    }

    TEST_F(SharedEnvironTest, shared2) {
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

A ``TmpDirFixture`` can only be instantiated at test scope. However, the
static ``run_path()`` method can be used to create a shared directory for
multiple tests across one or more test suites.


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
.. _share resources across tests: https://google.github.io/googletest/advanced.html#sharing-resources-between-tests-in-the-same-test-suite
