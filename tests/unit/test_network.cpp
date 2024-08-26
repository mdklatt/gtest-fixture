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
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>


using namespace testing::fixture::network;
using testing::fixture::shared::Shared;
using testing::Test;
using std::chrono::duration;
using std::reverse;
using std::string;
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
 * Test suite for the TcpClientFixture class
 */
class TcpClientFixtureTest: public Test {
protected:
    /**
     * Per-test setup.
     */
    TcpClientFixtureTest() {
        server.start();
    }

    TcpEchoHandler handler;
    TcpServerFixture server{handler};
};


/**
 * Test the TcpClientFixture::test_data() method.
 */
TEST_F(TcpClientFixtureTest, send_data) {
    // This also tests the host constructor.
    static const vector<char> data{'A', 'B', 'C'};
    TcpClientFixture client{"localhost", server.port()};
    EXPECT_EQ(data, client.send_data(data));
}


/**
 * Test TcpClientFixture communication via socket
 */
TEST_F(TcpClientFixtureTest, send_text) {
    static const string text{"ABC"};
    TcpClientFixture client{"localhost", server.port()};
    EXPECT_EQ("ABC", client.send_text(text));
}


/**
 * Test suite for the TcpServerFixture class.
 */
class TcpServerFixtureTest: public Test {
protected:
    TcpEchoHandler handler;
    TcpServerFixture fixture{handler};
};


/**
 * Test the TcpServerFixture::port() method.
 */
TEST_F(TcpServerFixtureTest, port) {
    static const auto port{8974};  // beware of existing usages
    TcpServerFixture fixture{handler, port};
    EXPECT_EQ(0, fixture.port());  // not running yet
    fixture.start();
    EXPECT_EQ(port, fixture.port());
}


/**
 * Test TcpServerFixture communication.
 */
TEST_F(TcpServerFixtureTest, comm) {
    fixture.start();
    EXPECT_NE(fixture.port(), 0);
    auto client{fixture.client()};  // caller must shutdown()
    const vector<char> request{'A', 'B', 'C'};
    send(client, request.data(), request.size(), 0);
    vector<char> buffer(256);
    static const duration<float> delay{0.5};  // seconds
    sleep_for(delay);  // wait for polling thread to complete
    auto count{recv(client, buffer.data(), buffer.size(), 0)};
    const Bytes response{buffer.data(), buffer.data() + count};
    EXPECT_EQ(response, request);
    shutdown(client, SHUT_RDWR);
    fixture.stop();
}


/**
 * Test TcpPortFixture with the Shared<> adaptor.
 */
TEST_F(TcpServerFixtureTest, shared) {
    Shared<TcpServerFixture> fixture{handler};
    EXPECT_EQ(fixture->port(), 0);  // not started
}


/**
 * Test suite for the TcpBufferHandler class.
 */
class TcpBufferHandlerTest: public Test {
protected:
    /**
     * Per-test setup.
     */
    TcpBufferHandlerTest() {
        // Test data concatenation.
        handler.receive(0, {'A', 'B', 'C'});
        handler.receive(1, {'D', 'E', 'F'});
    }

    TcpBufferHandler handler;
};


/**
 * Test the TcpBufferHandler::response() method.
 */
TEST_F(TcpBufferHandlerTest, response) {
    // Socket ID should not matter.
    ASSERT_FALSE(handler.response(-1));
}


/**
 * Test the TcpBufferHandler::clear() method.
 */
TEST_F(TcpBufferHandlerTest, clear) {
    handler.clear();
    ASSERT_EQ(handler.data(), Bytes{});
}


/**
 * Test the TcpBufferHandler::data() method.
 */
TEST_F(TcpBufferHandlerTest, data) {
    ASSERT_EQ(handler.data(), Bytes({'A', 'B', 'C', 'D', 'E', 'F'}));
}


/**
 * Test the TcpBufferHandler::text() method.
 */
TEST_F(TcpBufferHandlerTest, text) {
    ASSERT_EQ(handler.text(), "ABCDEF");
}


/**
 * Test suite for the TcpEchoHandler class.
 */
class TcpEchoHandlerTest: public Test {
protected:
    /**
     * Per-test setup.
     */
    TcpEchoHandlerTest():
        data{{'A', 'B', 'C'}, {{'D', 'E', 'F'}}} {
        int sock{0};
        for (const auto& item: data) {
            handler.receive(sock++, item);
        }
    }

    const vector<Bytes> data;
    TcpEchoHandler handler;
};


/**
 * Test the TcpEchoHandler::response() method.
 */
TEST_F(TcpEchoHandlerTest, response) {
    int sock{0};
    for (const auto& item: data) {
        ASSERT_EQ(handler.response(sock++), item);
    }
}


/**
 * Test the TcpEchoHandler::clear() method.
 */
TEST_F(TcpEchoHandlerTest, clear) {
    handler.clear();
    int sock{0};
    for (const auto& item: data) {
        ASSERT_FALSE(handler.response(sock++));
    }
}
