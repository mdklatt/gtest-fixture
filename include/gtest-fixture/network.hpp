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
 * Find an available TCP port on localhost.
 */
class TcpPortFixture {
public:
    /**
     * Initialize this instance to an available port.
     */
    TcpPortFixture();

    /**
     * Get the fixture's current port value.
     *
     * There is no guarantee that this port is still available for binding.
     * See reset() and bind().
     *
     * @return
     */
    operator in_port_t() const;

    /**
     * Reset the fixture's port value.
     *
     * This can be used to find a new port if the current port has become
     * unavailable.
     *
     * @return new port number
     */
     in_port_t reset();

    /**
     * Bind a TCP socket to the fixture's port value.
     *
     * This eliminates the race condition where the port may become unavailable
     * before it can be bound to a socket. The caller assumes ownership of the
     * socket.
     *
     * This will reset the fixture's current value as necessary to find an
     * available port.
     *
     * @return socket descriptor bound to the current port
     */
    int bind();

private:
    static const std::unique_ptr<addrinfo, void (*)(addrinfo*)> addr;
    in_port_t port{0};
};


/**
 * Run a simple TCP server for testing client services.
 */
class TcpServerFixture {
public:
    /**
     * Construct a new fixture instance.
     *
     * By default, the system will assign an available port number.
     *
     * @param callback: callback to execute when data is received
     * @return listening port
     */
    explicit TcpServerFixture(in_port_t port=0);

    /**
     * Destruct an instance.
     *
     * The original contents of the input stream will be restored.
     */
    virtual ~TcpServerFixture();

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
     * This returns 0 if the server has not been started yet.
     *
     * @return port number
     */
    in_port_t port() const;

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
     * Return all data received at the listening port as text.
     *
     * Data buffers are every time the server is started.
     *
     * @param client socket descriptor
     * @return text
     */
    std::string text() const;

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
    TcpServerFixture(const TcpServerFixture&) = delete;

private:
    int socket{-1};
    in_port_t port_{0};
    std::unique_ptr<addrinfo, void (*)(addrinfo*)> addr;
    std::future<void> serve;
    std::atomic<bool> stopped{true};
    std::vector<char> bytes;

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
