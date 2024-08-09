#include "gtest-fixture/network.hpp"
#include <chrono>
#include <future>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <netdb.h>
#include <poll.h>  // *nix only
#include <sys/socket.h>
#include <sys/types.h>


using std::async;
using std::future_status;
using std::chrono::duration;
using std::function;
using std::runtime_error;
using std::strerror;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
using testing::fixture::AvailablePort;
using testing::fixture::ServerFixture;


namespace {

/**
 * Create a local TCP socket address.
 *
 * @param port port number (0 to auto assign at bind time)
 * @return socket address
 */
unique_ptr<addrinfo, void (*)(addrinfo*)> create_address(in_port_t port=0) {
    const auto port_str{to_string(port)};
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* addr;
    int status;
    if ((status = getaddrinfo("localhost", port_str.c_str(), &hints, &addr)) != 0) {
        const auto error{gai_strerror(status)};
        throw runtime_error{"addrinfo error:" + string{error}};
    }
    return {addr, freeaddrinfo};
}


/**
 * Create a local TCP socket
 *
 * @param addr socket address
 * @return socket descriptor
 */
int create_socket(const addrinfo* addr) {
    auto sock{socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)};
    if (sock == -1) {
        const auto error{strerror(errno)};
        throw runtime_error{"socket error: " + string{error}};
    }
    static const int reuse{1};  // reuse port
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    return sock;
}


/**
 * Bind a TCP socket to a local port.
 *
 * @param addr socket address
 * @param sock socket descriptor
 * @return bound port number
 */
int bind_socket(int sock, const addrinfo* addr) {
    // Need to use getsockname() to get the port number for the case where
    // auto-assigment is used (port=0).
    if (bind(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
        const auto error{strerror(errno)};
        throw runtime_error{"bind error: " + string{error}};
    }
    sockaddr_in addr_in{};
    socklen_t len{sizeof(addr_in)};
    if (getsockname(sock, reinterpret_cast<sockaddr*>(&addr_in), &len) == -1) {
        const auto error{strerror(errno)};
        throw runtime_error{"getsockname error: " + string{error}};
    }
    return ntohs(addr_in.sin_port);
}

}  // internal linkage


const unique_ptr<addrinfo, void (*)(addrinfo*)> AvailablePort::addr{create_address()};


AvailablePort::operator in_port_t() const {
    return port;
}


in_port_t AvailablePort::reset() {
    // Temporarily bind a socket local socket to get its auto-assigned port
    // number. There is no guarantee that the port number will still be
    // available once the caller attempts to use that port.
    auto sock{create_socket(addr.get())};
    try {
        port = bind_socket(sock, addr.get());
    }
    catch (...) {
        shutdown(sock, SHUT_RDWR);
        throw;
    }
    shutdown(sock, SHUT_RDWR);
    return port;
}


int AvailablePort::bind() {
    size_t max_attempts{10};
    int sock{-1};
    while (max_attempts-- > 0) {
        // Attempt to bind the current port to a socket.
        const auto port_addr{create_address(port)};
        sock = create_socket(port_addr.get());
        if (::bind(sock, port_addr->ai_addr, port_addr->ai_addrlen) == -1) {
            if (errno == EADDRINUSE) {
                // Port is not free, try again.
                reset();
                continue;
            }
            const auto error{strerror(errno)};
            throw runtime_error{"bind error: " + string{error}};
        }
    }
    if (max_attempts == 0) {
        throw runtime_error{"could not find an available port"};
    }
    return sock;
}


AvailablePort::AvailablePort() {
    reset();
}


// Adapted from <https://beej.us/guide/bgnet/html/split-wide/system-calls-or-bust.html#system-calls-or-bust>.


ServerFixture::ServerFixture(in_port_t port):
    addr{create_address(port)} {}


ServerFixture::~ServerFixture() {
    stop();
}


in_port_t ServerFixture::port() const {
    return port_;
}


int ServerFixture::client() const {
    auto sock{create_socket(addr.get())};
    auto addr_in{reinterpret_cast<sockaddr_in*>(addr->ai_addr)};
    addr_in->sin_port = htons(port());
    if (::connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
        const auto error{strerror(errno)};
        throw runtime_error{"connect error: " + string{error}};
    }
    return sock;
}


const vector<char>& ServerFixture::data() const {
    return bytes;
}


void ServerFixture::start() {
    if (not stopped) {
        return;
    }
    static const duration<float> delay{0.5};  // seconds
    bytes.clear();
    socket = create_socket(addr.get());
    port_ = bind_socket(socket, addr.get());
    listen(socket, 0);  // only one concurrent connection allowed
    std::this_thread::sleep_for(delay);  // wait for socket to start listening
    stopped = false;
    serve = async([this]() {
        return poll();
    });
}


void ServerFixture::stop() {
    if (stopped) {
        return;
    }
    stopped = true;  // poll() will exit after its current loop
    serve.wait();
    if (socket != -1) {
        shutdown(socket, SHUT_RDWR);
        socket = -1;
    }
    port_ = 0;
}


void ServerFixture::poll() {
    pollfd listener{};
    listener.fd = socket;
    listener.events = POLLIN;
    vector<pollfd> sockets{listener};
    while (not stopped) {
        static const int timeout{30};  // seconds, -1 for no timeout
        auto poll_count{::poll(sockets.data(), sockets.size(), timeout)};
        if (poll_count == -1) {
            const auto error{strerror(errno)};
            throw runtime_error{"poll error: " + string{error}};
        }
        const auto socket_count{sockets.size()};  // vector is modified during iteration
        for (auto item{0}; item < socket_count; ++item) {
            const auto& sock{sockets[item]};
            if (sock.revents & POLLIN) {
                // New input is available.
                if (sock.fd == socket) {
                    // A new client is connected. It's okay to modify the
                    // container here because the original size is used for
                    // iteration. The new client will be polled during the next
                    // cycle.
                    pollfd client{};
                    client.fd = accept(socket);
                    client.events = POLLIN;
                    sockets.emplace_back(client);
                }
                else {
                    // Get data from a connected client.
                    read(sock.fd);
                }
            }
        }
    }
}


int ServerFixture::accept(int sock) {
    sockaddr_storage addr{};
    socklen_t len{sizeof(addr)};
    auto client_sock{::accept(sock, reinterpret_cast<sockaddr*>(&addr), &len)};
    if (client_sock == -1) {
        const auto error{strerror(errno)};
        throw runtime_error{"client accept error: " + string{error}};
    }
    return client_sock;
}


void ServerFixture::read(int sock) {
    // There is no way to match this side of the connection (`sock`) to the
    // caller's side, e.g. the return value of `client()`. Therefore, there is
    // no point in maintaining a separate buffer for each client connection.
    vector<char> buffer(256);
    ssize_t count;
    do {
        count = recv(sock, buffer.data(), buffer.size(), 0);
        bytes.insert(bytes.end(), buffer.begin(), buffer.begin() + count);
    } while (count > 0);
}
