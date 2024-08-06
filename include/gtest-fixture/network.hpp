/**
 * Capture output streams for inspection.
 */
#ifndef GTEST_FIXTURE_NETWORK_HPP
#define GTEST_FIXTURE_NETWORK_HPP

#include <atomic>
#include <functional>
#include <future>
#include <vector>
#include <netdb.h>


namespace testing::fixture {

/**
 * listen for data from a local TCP socket.
 */
class ServerFixture {
public:
    /**
     * Construct a new fixture instance.
     *
     * By default, the system will assign an available port number.
     *
     * @param callback: callback to execute when data is received
     * @return listening port
     */
    explicit ServerFixture(in_port_t port=0);

    /**
     * Destruct an instance.
     *
     * The original contents of the input stream will be restored.
     */
    virtual ~ServerFixture();

    /**
     * Create a client socket for communicating with the fixture.
     *
     * The caller is responsible for calling shutdown() on the socket.
     *
     * @return connected socket descriptor
     */
     int client() const;

    /**
     * The port number the server is listening on.
     *
     * @return port number
     */
    int port() const;

    /**
     * Return all data received at the listening port.
     *
     * Data buffers are every time the server is started.
     *
     * @param client socket descriptor
     * @return bytes
     */
    const std::vector<char>& data() const;

    /**
     * Start the server asynchronously.
     *
     * This is nonblocking. The server will listen in a separate thread until
     * stop() is called.
     */
    void start();

    /**
     * Stop the server.
     */
    void stop();


    // Prohibited operations.
    ServerFixture(const ServerFixture&) = delete;

private:
    int socket{-1};
    std::unique_ptr<addrinfo, void (*)(addrinfo*)> addr;
    std::future<void> serve;
    std::atomic<bool> stopped{true};
    std::vector<char> bytes;

    /**
     * Create the listening socket connection.
     *
     * @param addr: address object
     * @return server socket descriptor
     */
    void connect(const addrinfo* addr);

    /**
     * Accept a connection from a client.
     *
     * @param sock server socket descriptor
     * @return client socket descriptor
     */
    int accept(int sock);

    /**
     * Handle communication with active sockets.
     *
     * This is intended to be run in its own thread.
     */
    void poll();

    /**
     * Read data from a socket and append it to its buffer.
     *
     * @param sock
     */
    void read(int sock);
};

}  // testing::fixture


#endif  // GTEST_FIXTURE_NETWORK_HPP
