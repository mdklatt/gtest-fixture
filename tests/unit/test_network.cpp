/**
 * Test suite for the 'network' module.
 *
 * Link all test files with the `gtest_main` library to create a command-line
 * test runner.
 */
#include "gtest-infra/network.hpp"
#include "gtest-infra/shared.hpp"
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>


using namespace testing::infra::network;
using testing::infra::shared::Shared;
using testing::Test;
using std::chrono::duration;
using std::reverse;
using std::string;
using std::this_thread::sleep_for;
using std::vector;


/**
 * Test suite for the TcpPort class.
 */
class TcpPortTest: public Test {
protected:
    TcpPort port;
};


/**
 * Test the TcpPort default constructor.
 */
TEST_F(TcpPortTest, ctor) {
    EXPECT_NE(port, 0);
}


/**
 * Test the TcpPort::reset() method.
 */
TEST_F(TcpPortTest, reset) {
    const auto save{port};
    EXPECT_GE(port.reset(), 0);
    EXPECT_NE(port, save);
}


/**
 * Test the TcpPort::bind() method.
 */
TEST_F(TcpPortTest, bind) {
    const auto sock{port.bind()};
    EXPECT_GE(sock, 0);
    shutdown(sock, SHUT_RDWR);
}


/**
 * Test TcpPort with the Shared<> adaptor.
 */
TEST_F(TcpPortTest, shared) {
    Shared<TcpPort> fixture;
    EXPECT_NE(fixture->operator in_port_t(), 0);
    fixture.teardown();
}


/**
 * Test suite for the TcpClient class
 */
class TcpClientTest: public Test {
protected:
    /**
     * Per-test setup.
     */
    TcpClientTest() {
        server.start();
    }

    TcpEchoHandler handler;
    TcpServer server{handler};
};


/**
 * Test the TcpClient::test_data() method.
 */
TEST_F(TcpClientTest, send_data) {
    // This also tests the host constructor.
    static const vector<char> data{'A', 'B', 'C'};
    TcpClient client{"localhost", server.port()};
    EXPECT_EQ(data, client.send_data(data));
}


/**
 * Test TcpClient communication via socket
 */
TEST_F(TcpClientTest, send_text) {
    static const string text{"ABC"};
    TcpClient client{"localhost", server.port()};
    EXPECT_EQ("ABC", client.send_text(text));
}


/**
 * Test suite for the TcpServer class.
 */
class TcpServerTest: public Test {
protected:
    TcpEchoHandler handler;
    TcpServer fixture{handler};
};


/**
 * Test the TcpServer::port() method.
 */
TEST_F(TcpServerTest, port) {
    static const auto port{8974};  // beware of existing usages
    TcpServer fixture{handler, port};
    EXPECT_EQ(0, fixture.port());  // not running yet
    fixture.start();
    EXPECT_EQ(port, fixture.port());
}


/**
 * Test TcpServer communication.
 */
TEST_F(TcpServerTest, comm) {
    fixture.start();
    EXPECT_NE(fixture.port(), 0);
    auto client{fixture.client()};  // caller must shutdown()
    const Bytes request{'A', 'B', 'C'};
    send(client, request.data(), request.size(), 0);
    vector<char> buffer(256);
    static const duration<float> delay{0.5};  // seconds
    sleep_for(delay);  // wait for polling thread to complete
    auto count{recv(client, buffer.data(), buffer.size(), 0)};
    const Bytes data{buffer.data(), buffer.data() + count};
    const Bytes response{data};
    EXPECT_EQ(response, request);
    shutdown(client, SHUT_RDWR);
    fixture.stop();
}


/**
 * Test TcpPort with the Shared<> adaptor.
 */
TEST_F(TcpServerTest, shared) {
    Shared<TcpServer> fixture{handler};
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
