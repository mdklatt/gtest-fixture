/**
 * Test suite for the 'network' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-fixture/network.hpp"
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
 * Test suite for the EnvironFixture class.
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
    ASSERT_EQ(0, fixture.port());  // not running yet
    fixture.start();
    ASSERT_EQ(port, fixture.port());
}


/**
 * Test ServerFixture communication.
 */
TEST_F(ServerFixtureTest, comm) {
    fixture.start();
    ASSERT_NE(fixture.port(), 0);
    auto client{fixture.client()};  // caller must shutdown()
    const vector<char> bytes{'T', 'E', 'S', 'T'};
    send(client, bytes.data(), bytes.size(), 0);
    static const duration<float> delay{1};  // seconds
    sleep_for(delay);  // wait for polling thread to complete
    shutdown(client, SHUT_RDWR);
    fixture.stop();
    EXPECT_EQ(bytes, fixture.data());
}
