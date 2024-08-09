/**
 * Test suite for the 'network' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/network.hpp"
#include "gtest-fixture/shared.hpp"
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <chrono>
#include <vector>

using namespace testing::fixture;
using testing::Test;
using std::chrono::duration;
using std::this_thread::sleep_for;
using std::vector;


/**
 * Test suite for the TcpPortFixture class.
 */
class TcpPortFixtureTest: public Test {
protected:
    TcpPortFixture port;
};


/**
 * Test the TcpPortFixture default constructor.
 */
TEST_F(TcpPortFixtureTest, ctor) {
    EXPECT_NE(port, 0);
}


/**
 * Test the TcpPortFixture::reset() method.
 */
TEST_F(TcpPortFixtureTest, reset) {
    const auto save{port};
    EXPECT_GE(port.reset(), 0);
    EXPECT_NE(port, save);
}


/**
 * Test the TcpPortFixture::bind() method.
 */
TEST_F(TcpPortFixtureTest, bind) {
    const auto sock{port.bind()};
    EXPECT_GE(sock, 0);
    shutdown(sock, SHUT_RDWR);
}


/**
 * Test TcpPortFixture with the Shared<> adaptor.
 */
TEST_F(TcpPortFixtureTest, shared) {
    Shared<TcpPortFixture> fixture;
    EXPECT_NE(fixture->operator in_port_t(), 0);
    fixture.teardown();
}


/**
 * Test suite for the ServerFixture class.
 */
class ServerFixtureTest: public Test {
protected:
    vector<char> buffer;
    ServerFixture fixture;
};


/**
 * Test the ServerFixture::port() method.
 */
TEST_F(ServerFixtureTest, port) {
    static const auto port{8974};  // beware of existing usages
    ServerFixture fixture{port};
    EXPECT_EQ(0, fixture.port());  // not running yet
    fixture.start();
    EXPECT_EQ(port, fixture.port());
}


/**
 * Test ServerFixture communication.
 */
TEST_F(ServerFixtureTest, comm) {
    fixture.start();
    EXPECT_NE(fixture.port(), 0);
    auto client{fixture.client()};  // caller must shutdown()
    const vector<char> bytes{'T', 'E', 'S', 'T'};
    send(client, bytes.data(), bytes.size(), 0);
    static const duration<float> delay{1};  // seconds
    sleep_for(delay);  // wait for polling thread to complete
    shutdown(client, SHUT_RDWR);
    fixture.stop();
    EXPECT_EQ(bytes, fixture.data());
}


/**
 * Test TcpPortFixture with the Shared<> adaptor.
 */
TEST_F(ServerFixtureTest, shared) {
    Shared<ServerFixture> fixture;
    EXPECT_EQ(fixture->port(), 0);
}
