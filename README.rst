#############
gtest-fixture
#############

Test fixtures for use with `GoogleTest`_.


=====
Usage
=====

Use these fixtures with ``testing::Test`` classes to manage resources during
testing.


Fixtures
--------

``EnvironFixture``

Modify environment variables for testing. Changes will be rolled back when the
fixture is destroyed. Note that changes to the environment are *not*
thread-safe.



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


.. _GoogleTest: http://google.github.io/googletest/
