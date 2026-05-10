#include "gtest-infra/network.hpp"
#include <cassert>
#include <cmath>
#include <cstring>
#include <chrono>
#include <future>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <netdb.h>
#include <poll.h>  // *nix only
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>


using std::async;
using std::modf;
using std::future_status;
using std::chrono::duration;
using std::function;
using std::optional;
using std::runtime_error;
using std::strerror;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
using testing::infra::network::SOCKET_TIMEOUT;
using testing::infra::network::Bytes;
using testing::infra::network::TcpPortFixture;
using testing::infra::network::TcpClientFixture;
using testing::infra::network::TcpServerFixture;
using testing::infra::network::TcpClientHandler;
using testing::infra::network::TcpBufferHandler;
using testing::infra::network::TcpEchoHandler;


namespace {

// Adapted from "Beej's Guide to Network Programming".
// <https://beej.us/guide/bgnet/html/split-wide/system-calls-or-bust.html#system-calls-or-bust>

/**
 * Create a TCP socket address.
 *
 * @param host: target hostname
 * @param port: target port
 * @return socket address
 */
unique_ptr<addrinfo, void (*)(addrinfo *)> create_address(const string& host, in_port_t port) {
    const auto port_str{to_string(port)};
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo *addr;
    int status;
    if ((status = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &addr)) != 0) {
        const auto error{gai_strerror(status)};
        throw runtime_error{"addrinfo error:" + string{error}};
    }
    return {addr, freeaddrinfo};
}


/**
 * Create a TCP socket address on localhost.
 *
 * @param port port number (0 to auto assign at bind time)
 * @return socket address
 */
unique_ptr<addrinfo, void (*)(addrinfo *)> create_address(in_port_t port = 0) {
    return create_address("localhost", port);
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
    float sec;
    const float usec{1e6f * modf(SOCKET_TIMEOUT, &sec)};
    timeval timeout{};
    timeout.tv_sec = static_cast<int>(sec);
    timeout.tv_usec = static_cast<int>(usec);
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    static const int reuse{1};
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    return sock;
}


/**
 * Connect a socket to a host address.
 *
 * @param sock socket descriptor
 * @param addr target address
 */
void connect_socket(int sock, const addrinfo* addr) {
    if (connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
        const auto error{strerror(errno)};
        throw runtime_error{"connect error: " + string{error}};
    }
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


/**
 * Send data over a connected socket.
 *
 * @param sock socket descriptor
 * @param data bytes to send
 */
void send_socket(int sock, Bytes data) {
    while (not data.empty()) {
        // Continue until all data has been sent.
        const auto count{send(sock, data.data(), data.size(), 0)};
        if (count == -1) {
            const auto error{strerror(errno)};
            throw runtime_error{"send error: " + string{error}};
        }
        data.assign(data.begin() + count, data.end());  // trim sent data
    }
}


/**
 * Read data over a connected socket.
 *
 * @param sock
 * @return data received from socket
 */
Bytes read_socket(int sock) {
    static vector<char> buffer(3);
    Bytes response;
    ssize_t count;
    do {
        // Request more data as long as the read buffer was filled. If the
        // number of bytes being sent is an exact multiple of buffer.size(),
        // there will be 0 bytes to read on the last iteration, which will
        // cause recv() to time out.
        count = ::recv(sock, buffer.data(), buffer.size(), 0);
        if (count == -1) {
            if (errno == EWOULDBLOCK or errno == EAGAIN) {
                // One reason for this is a recv() timeout. In any case, return
                // all the data received so far.
                break;
            }
            const auto error{strerror(errno)};
            throw runtime_error{"read error: " + string{error}};
        }
        response.insert(response.end(), buffer.begin(), buffer.begin() + count);
    }
    while (count == buffer.size());
    return response;
}

}  // internal linkage


float testing::infra::network::SOCKET_TIMEOUT = 1;


const unique_ptr<addrinfo, void (*)(addrinfo*)> TcpPortFixture::addr{create_address()};


TcpPortFixture::operator in_port_t() const {
    return port;
}


in_port_t TcpPortFixture::reset() {
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


int TcpPortFixture::bind() {
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


TcpPortFixture::TcpPortFixture() {
    reset();
}


TcpClientFixture::TcpClientFixture(const string& host, in_port_t port):
    addr{create_address(host, port)} {}


Bytes TcpClientFixture::send_data(const Bytes& data) {
    Bytes response;
    const auto sock{create_socket(addr.get())};
    try {
        connect_socket(sock, addr.get());
        send_socket(sock, data);
        response = read_socket(sock);
    }
    catch (...) {
        shutdown(sock, SHUT_RDWR);
        throw;
    }
    shutdown(sock, SHUT_RDWR);
    return response;
}


string TcpClientFixture::send_text(const string& text) {
    const Bytes data{text.begin(), text.end()};
    const auto response{send_data(data)};
    return {response.begin(), response.end()};
}


optional<Bytes> TcpClientHandler::response(int sock) {
    return {};
}


void TcpBufferHandler::receive(int sock, const Bytes& data) {
    buffer.insert(buffer.end(), data.begin(), data.end());
}


void testing::infra::network::TcpBufferHandler::clear() {
    buffer.clear();
}


const Bytes& TcpBufferHandler::data() const {
    return buffer;
}


string TcpBufferHandler::text() const {
    return {buffer.begin(), buffer.end()};
}


void TcpEchoHandler::receive(int sock, const Bytes& data) {
    auto& buffer{buffers[sock]};
    buffer.insert(buffer.end(), data.begin(), data.end());
}


optional<Bytes> TcpEchoHandler::response(int sock) {
    static const optional<Bytes> null;
    const auto it{buffers.find(sock)};
    if (it == buffers.end()) {
        return null;
    }
    auto response{it->second};
    buffers.erase(it);
    return response;
}


void TcpEchoHandler::clear() {
    buffers.clear();
}


TcpServerFixture::TcpServerFixture(TcpClientHandler& handler, in_port_t port):
    handler{&handler},
    addr{create_address(port)} {}


TcpServerFixture::~TcpServerFixture() {
    stop();
}


in_port_t TcpServerFixture::port() const {
    return listen_port;
}


int TcpServerFixture::client() const {
    auto port_addr{create_address(listen_port)};
    auto sock{create_socket(port_addr.get())};
    connect_socket(sock, port_addr.get());
    return sock;
}


void TcpServerFixture::start() {
    if (not stopped) {
        return;
    }
    static const duration<float> delay{0.5};  // seconds
    listen_sock = create_socket(addr.get());
    listen_port = bind_socket(listen_sock, addr.get());
    listen(listen_sock, 0);  // only one concurrent connection allowed
    std::this_thread::sleep_for(delay);  // wait for socket to start listening
    stopped = false;
    serve = async([this]() {
        return poll();
    });
}


void TcpServerFixture::stop() {
    if (stopped) {
        return;
    }
    stopped = true;  // poll() will exit after its current loop
    serve.wait();
    if (listen_sock != -1) {
        shutdown(listen_sock, SHUT_RDWR);
        listen_sock = -1;
    }
    listen_port = 0;
}


void TcpServerFixture::poll() {
    pollfd listener{};
    listener.fd = listen_sock;
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
            auto& sock{sockets[item]};
            if (sock.revents & POLLIN) {
                // New input is available.
                if (sock.fd == listen_sock) {
                    // A new client is connected. It's okay to modify the
                    // container here because the original size is used for
                    // iteration. The new client will be polled during the next
                    // cycle.
                    pollfd client{};
                    client.fd = accept(listen_sock);
                    client.events = POLLIN;
                    sockets.emplace_back(client);
                }
                else {
                    // Get data from a connected client. If the size of the
                    // sent data is greater than buffer.size(), the remaining
                    // data will be available on the next iteration.
                    assert(handler);
                    static Bytes buffer(1024);
                    const auto count{recv(sock.fd, buffer.data(), buffer.size(), 0)};
                    if (count == -1) {
                        const auto error{strerror(errno)};
                        throw runtime_error{"read error: " + string{error}};
                    }
                    handler->receive(sock.fd, {buffer.begin(), buffer.begin() + count});
                    const auto response{handler->response(sock.fd)};
                    if (response) {
                        send_socket(sock.fd, response.value());
                    }
                }
            }
        }
    }
}


int TcpServerFixture::accept(int sock) {
    sockaddr_storage addr{};
    socklen_t len{sizeof(addr)};
    auto client_sock{::accept(sock, reinterpret_cast<sockaddr*>(&addr), &len)};
    if (client_sock == -1) {
        const auto error{strerror(errno)};
        throw runtime_error{"client accept error: " + string{error}};
    }
    return client_sock;
}
