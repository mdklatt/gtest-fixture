/**
 * Capture output streams for inspection.
 */
#ifndef GTEST_INFRA_NETWORK_HPP
#define GTEST_INFRA_NETWORK_HPP

#include <atomic>
#include <functional>
#include <future>
#include <map>
#include <optional>
#include <vector>
#include <netdb.h>


namespace testing::infra::network {


using Bytes = std::vector<char>;


extern float SOCKET_TIMEOUT;  // seconds

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
 * Communicate with a TCP server under test.
 */
class TcpClientFixture {
public:
    /**
     * Construct a new instance from a target address.
     *
     * @param host target hostname
     * @param port target port
     */
    TcpClientFixture(const std::string& host, in_port_t port);

    /**
     * Send data to the server.
     *
     * @param data bytes to send
     * @return data returned to server
     */
    Bytes send_data(const Bytes& data);

    /**
     * Send text to the server.
     *
     * @param text text to send
     * @return text returned from server
     */
    std::string send_text(const std::string& text);

private:
    const std::unique_ptr<addrinfo, void (*)(addrinfo*)> addr;
};


/**
 * Interface for handling incoming requests to TcpServerFixture.
 */
class TcpClientHandler {
public:
    /**
     * Receive data from a client connection.
     *
     * This is called every time a new chunk of data is received from the
     * client. It should not be assumed that each chunk of data represents a
     * complete request. It is the derived class's responsibility to buffer
     * input and assemble it into complete requests.
     *
     * The socket ID can used to track input from multiple connections. This
     * is the socket for the server's connection to the client; there is no way
     * to match this to the client's side of the connection.
     *
     * @param sock client connection socket ID
     * @param data data received from client
     */
    virtual void receive(int sock, const Bytes& data) = 0;

    /**
     * Optionally return a response to a client connection.
     *
     * This is called once the client connection reports EOF, but this does not
     * guarantee that a complete request has been received. It is the derived
     * class's responsibility to buffer input and generate a response when
     * appropriate. The default  implementation returns a null response that is
     * ignored by TcpServerFixture.
     *
     * @param sock client connection socket ID
     * @return client response
     */
    virtual std::optional<Bytes> response(int sock);

protected:
    /**
     * Default constructor.
     */
    TcpClientHandler() = default;
};


/**
 * Buffer input for inspection.
 */
class TcpBufferHandler: public TcpClientHandler {
public:
    /**
     * Received data from a client connection.
     *
     * All data is placed into a single buffer regardless of which connection
     * it was received from.
     *
     * @param sock client connection socket ID
     * @param data data received from client
     */
    void receive(int sock, const Bytes& data) override;

    /**
     * Clear buffered data.
     */
    void clear();

    /**
     * Return all data received.
     *
     * @return data
     */
    const Bytes& data() const;

    /**
     * Return all data received as a string.
     *
     * @return text
     */
    std::string text() const;

private:
    Bytes buffer;
};


/**
 * Echo input on a client connection.
 */
class TcpEchoHandler: public TcpClientHandler {
public:
    /**
     * Clear buffered data.
     */
    void clear();

public:  // TcpClientHandler interface
    /**
     * Receive input from a client connection.
     *
     * @param sock client connection socket ID
     * @param data data received from client
     */
    void receive(int sock, const Bytes& data) override;

    /**
     * Echo all input received on a client connection.
     *
     * A null response that is ignored by TcpServerFixture is returned if there
     * is no input for the given client connection.
     *
     * @param sock client connection socket ID
     * @return data received from client
     */
    std::optional<Bytes> response(int sock) override;

private:
    std::map<int, Bytes> buffers;
};


/**
 * Run a simple TCP server for clients under test.
 */
class TcpServerFixture {
public:
    /**
     * Construct a new instance.
     *
     * By default, the system will assign an available port number.
     *
     * @return listening port
     */
    explicit TcpServerFixture(TcpClientHandler& handler, in_port_t port=0);

    /**
     * Destruct an instance.
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
     * Start the server asynchronously.
     *
     * This is nonblocking. The server will listen in a separate thread until
     * stop() is called. This will reset all data received buffers.
     */
    void start();

    /**
     * Stop the server.
     */
    void stop();

    // Prohibited operations.
    TcpServerFixture(const TcpServerFixture&) = delete;

private:
    TcpClientHandler* handler{nullptr};
    int listen_sock{-1};
    in_port_t listen_port{0};
    std::unique_ptr<addrinfo, void (*)(addrinfo*)> addr;
    std::future<void> serve;
    std::atomic<bool> stopped{true};

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
     * @return true if more data is available for read
     */
    //bool read(int sock);
};

}  // testing::infra::network


#endif  // GTEST_INFRA_NETWORK_HPP
