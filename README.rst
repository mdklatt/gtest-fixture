###########
gtest-infra
###########

|release|
|license|
|tests|
|c++17|
|cmake|

Overview
========

*gtest-infra* is a C++ library that provides infrastructure components
for use in `GoogleTest`_ tests and fixtures. Typical use cases include creating
temporary directories, managing environment variables, capturing output, and
testing network communication. RAII is used to manage global state and isolate
changes within individual tests or test suites.


Demonstration
-------------

Given a "Hello World" function that optionally gets the recipient from an
environment variable and writes a greeting to ``STDOUT``, write unit tests for
the default and optional cases.

Function Under Test
~~~~~~~~~~~~~~~~~~~
.. code-block:: cpp

    /**
     * Display a greeting. The recipient defaults to "World", or specify a
     * different recipient using `$HELLO_RECIPIENT`.
     */
    void hello() {
        const auto* env_val{std::getenv("HELLO_RECIPIENT")};
        const std::string name{env_val ? *env_val : "World"};
        std::cout << "Hello, " << name  << "!" << std::endl;
    }

Unit Tests
~~~~~~~~~~
.. code-block:: cpp

    // gtest-infra headers
    #include <gtest-infra/environ.hpp>
    #include <gtest-infra/stream.hpp>
    #include <gtest-infra/tmpdir.hpp>

    using testing::Test;
    using testing::infra::environ::Environ;
    using testing::infra::stream::OutputStream;
    using testing::infra::tmpdir::TmpDir;

    /**
     * Test target for the hello() unit test suite.
     */
    class HelloTest: public testing::Test {
    protected:
        /**
         * Per-test setup.
         */
        HelloTest():
            stdout_path{tmpdir.test_path() / "stdout.txt"};
            stdout_file{stdout_path},
            stdout{std::cout, output_file} {}

        /**
         * Read captured output for STDOUT.
         *
         * @return STDOUT contents
         */
        std::string read_stdout() const {
            // Note that TmpDir does not delete the latest directories after a
            // test run, so output files are available for manual inspection.
            std::ifstream file{stdout_path};
            std::istringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

        std::string
        TmpDir tmpdir;
        const std::filesystem::path stdout_path;
        std::ofstream stdout_file;
        OutputStream stdout;  // RAII only, not accessed directly
    };

    /**
     * Test the hello() function with the default recipient.
     */
    TEST_F(HelloTest, default) {
        hello();
        ASSERT_EQ("Hello, World!", read_stdout());
    }

    /**
     * Test the hello() function with the $HELLO_RECIPIENT variable.
     */
    TEST_F(HelloTest, environ) {
        static const std::string name{"J. Doe"};
        Environ environ;
        environ.set("HELLO_RECIPIENT", name);  // resets between tests
        hello();
        const auto greeting{"Hello, " + name + "!"};
        ASSERT_EQ(greeting, read_stdout());
    }


Library Usage
=============

*GoogleTest* provides three levels of scoping for test isolation: per test, per
test suite, and per application. In most situations, *gtest-infra* components
should be given test scope by using them as a non-static member of a ``Test``
class or inside a ``TEST*`` function.


Available Modules
-----------------

testing::infra::environ
~~~~~~~~~~~~~~~~~~~~~~~
``Environ``
    Allow environment variables to set, unset, or modified for testing. Changes
    are rolled back when the ``Environ`` object goes out of scope. *Environment
    variable changes are not thread-safe.*


testing::infra::network
~~~~~~~~~~~~~~~~~~~~~~~
``TcpPort``
    Bind to an available TCP port on ``localhost``.

``TcpClient``
    Communicate with a TCP server under test.

``TcpServer``
    Create a TCP server for interacting with TCP clients under test.
    Concrete ``TcpClientHandler`` instances are define server responses to
    incoming requests.


testing::infra::shared
~~~~~~~~~~~~~~~~~~~~~~
``Shared<Infra>``
    Adaptor class for sharing resources across multiple tests using ``static``
    scoping. Resource can be shared `across a test suite`_ or
    `across the application`_.

    Shared resources might not work as expected when using `CTest`_ with the
    `GoogleTest module`_. In that case, tests are run individually rather than
    as a single executable, and thus they cannot depend on side effects from
    another test.


testing::infra::stream
~~~~~~~~~~~~~~~~~~~~~~
``InputStream``
    Simulate input to a ``std::istream`` instance under test. Compatibility
    with C streams is system-dependent.

``OutputStream``
    Capture output to a ``std::ostream`` instance under test. Compatibility
    with C streams is system-dependent.


testing::inftra::tmpdir
~~~~~~~~~~~~~~~~~~~~~~~
``TmpDir``
    Create a temporary directory for a test. Test directories are not
    immediately removed after a test run so that files can be inspected
    manually. Old test directories will be removed automatically.

    A ``TmpDir`` can only be instantiated at test scope, so it should not be
    used with the ``Shared<>`` adaptor. However, the static ``run_path()``
    method can be used to create a shared directory for multiple tests across
    one or more test suites.


Installation
------------

*GoogleTest* recommends building the library from source as part of the project
under test rather than using a prebuilt binary. This ensures that tests are
using the same build type, compiler setting, *etc.* as the project binaries.
This recommendation also applies to *gtest-infra*.



Install using Conan
~~~~~~~~~~~~~~~~~~~

Install the library from source as *Conan* package and configure the test
project to use *Conan* presets. This installs *GoogleTest* as a transitive
dependency.

.. code-block:: python

    """ Minimal Conanfile for a project that uses `gtest-infra`.

    """
    from conan import ConanFile
    from conan.tools.cmake import cmake_layout


    class ConsumerRecipe(ConanFile):
        """ Consumer project using gtest-infra and Google Test.

        """
        settings = "os", "arch", "compiler", "build_type"
        generators = "CMakeToolchain", "CMakeDeps"

        def requirements(self):
            """ Define project requirements. """
            self.requires("gtest-infra/1.0.0")
            self.requires("gtest/1.14.0")
            return

        def layout(self):
            """ Define project layout. """
            cmake_layout(self)
            return


.. code-block:: shell

    conan create https://github.com/mdklatt/gtest-infra.git#v1.0.0 --build=missing
    conan install --build=missing --settings=build_type=Debug
    cmake --preset conan-debug


.. code-block:: cmake

    # tests/CMakeLists.txt
    # Locate dependencies for use with `target_link_library()`.

    find_package(GTest REQUIRED)
    find_package(GTestFixture REQUIRED)


Install using vcpkg
~~~~~~~~~~~~~~~~~~~

Install the library to the local *vcpkg* installation and configure the test
project to use the *vcpkg* toolchain. This installs *GoogleTest* as a
transitive dependency.

.. code-block:: json

    {
      "name": "my-project",
      "version": "1.0.0",
      "dependencies": [
        "gtest-infra"
      ]
    }

.. code-block:: shell

    pushd vcpkg/ports
    git clone --branch=v1.0.0 --depth=1 https://github.com/mdklatt/gtest-infra.git
    popd
    vcpkg/vcpkg install
    cmake -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -B build/ -S .


.. code-block:: cmake

    # tests/CMakeLists.txt
    # Locate dependencies for use with `target_link_library()`.

    find_package(GTest REQUIRED)
    find_package(GTestFixture REQUIRED)


Install using CMake FetchContent
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Install *gtest-infra* and a compatible version of *GoogleTest* explicitly using
CMake `FetchContent`_.

.. code-block:: cmake

    # tests/CMakeLists.txt
    # Locate dependencies for use with `target_link_library()`.

    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(gtest-target
        GIT_REPOSITORY https://github.com/mdklatt/gtest-infra.git
        GIT_TAG v1.0.0
    )
    FetchContent_MakeAvailable(gtest-target)


Install as a CMake external build tree
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add the library and a compatible version of *GoogleTest* explicitly as
external source trees that will be built as part of the project.

.. code-block:: shell

   git clone --branch=v1.14.0 --depth=1  https://github.com/google/googletest.git vendor/googletest
   git clone --branch=v1.0.0 --depth=1 https://github.com/mdklatt/gtest-infra.git vendor/gtest-infra


.. code-block:: cmake

    # tests/CMakeLists.txt
    # Locate dependencies for use with `target_link_library()`.

    add_subdirectory(vendor/googletest)
    add_subdirectory(vendor/gtest-infra)


CMake Integration
-----------------

.. code-block:: cmake

    # test/CMakeLists.txt

    # Locate dependencies.

    # Build GoogleTest executable.
    add_executable(test_app
        # Test sources must not contain main().
        test_app.cpp
    )
    target_link_libraries(test_app
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTestInfra::gtest-infra
    )


GoogleTest Compatibility
------------------------

While the library is intended to be used with *GoogleTest*,  most of the
module do not have any *GoogleTest* dependencies and could be used with any
test framework. The exception is the ``tmpdir`` module, which uses the
`GoogleTest`_ API to get the current test name. This creates a build
dependency on *gtest*. *gtest-target* does not specify a pinned version
of *GoogleTest*, just a minimum (see ``conanfile.py``). *GoogleTest* does not
use Sematic Versioning, so beware that even a minor version change may break
backwards compatibility.



Developer Quickstart
====================

Clone the project.

.. code-block:: shell

    git clone https://github.com/mdklatt/gtest-infra.git
    cd gtest-infra
    git checkout dev

Set up the development environment and build the project with *Conan*
integration.

.. code-block:: shell

    make build


Run all project tests.

.. code-block:: shell

    make test


Build library documentation.

.. code-block:: shell

    make docs


Using CLion
-----------

For best results, set up the development environment before opening the project
in `CLion`_. It will recognize the ``CMakeUserPresets.json`` file created by
*Conan* and automatically create build configurations for the ``conan``
presets. These configurations will have to be manually enabled in the
*Build, Execution, Deployment > CMake* library


Project Layout
--------------

.. code-block:: text

    ├── .github/                      # GitHub Actions workflows
    ├── build/                        # build directory (generated)
    ├── cmake/                        # CMake configuration files
    ├── docs/                         # documentation
    ├── include/googletest-infra/     # public header files
    ├── src/                          # library implementation
    ├── tests/                        # project test suites
    │   └── unit/                     # C++ unit tests (GoogleTest)
    │   └── integration/              # integration tests (pytest)
    ├── venv                          # Python venv for running dev tools (generated)
    ├── CMakeLists.txt                # project CMake configuration
    ├── Makefile                      # project management tasks
    ├── requirements-env.txt          # Python requirements for the dev environment
    ├── version.txt                   # project version
    ├── conanfile.py                  # Conan configuration
    ├── vcpkg.json.in                 # template for vcpkg.json generation
    ├── vcpkg.json                    # vcpkg configuration for consumers (generated)
    ├── .gitignore                    # Git ignore file
    └── LICENSE                       # project license


.. |release| image:: https://img.shields.io/github/v/release/mdklatt/gtest-infra?sort=semver
    :alt: GitHub release (latest SemVer)
.. |license| image:: https://img.shields.io/github/license/mdklatt/gtest-infra
   :alt: MIT License
   :target: `MIT License`_
.. |tests| image:: https://github.com/mdklatt/gtest-infra/actions/workflows/test.yml/badge.svg
   :alt: CI Test
   :target: `GitHub Actions`_
.. |c++17| image:: https://img.shields.io/static/v1?label=c%2B%2B&message=17&color=informational
   :alt: C++17
.. |cmake| image:: https://img.shields.io/static/v1?label=cmake&message=3.16&color=informational
   :alt: CMake 3.16

.. _CMake: https://cmake.org/cmake/help/latest/
.. _CTest: https://cmake.org/cmake/help/latest/manual/ctest.1.html
.. _GitHub Actions: https://github.com/mdklatt/gtest-infra/actions/workflows/test.yml
.. _GoogleTest: http://google.github.io/googletest/
.. _GoogleTest module: https://cmake.org/cmake/help/latest/module/GoogleTest.html
.. _MIT License: http://choosealicense.com/licenses/mit
.. _across a test suite: https://google.github.io/googletest/advanced.html#sharing-resources-between-tests-in-the-same-test-suite
.. _across the application: https://google.github.io/googletest/advanced.html#global-set-up-and-tear-down
.. _FetchContent: https://cmake.org/cmake/help/v3.16/module/FetchContent.html
.. _CLion: https://www.jetbrains.com/clion/
