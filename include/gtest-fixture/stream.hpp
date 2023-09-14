/**
 * Capture output streams for inspection.
 */
#ifndef GTEST_FIXTURE_STREAM_HPP
#define GTEST_FIXTURE_STREAM_HPP

#include "shared.hpp"
#include <filesystem>
#include <memory>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <vector>


namespace testing::fixture {

/**
 * Replace the contents of an input stream.
 */
class InputFixture {
public:
    /**
     * Construct a new instance.
     *
     * @param stream stream to capture
     * @param input new stream contents
     */
    InputFixture(std::istream& stream, std::istream& input);

    /**
     * Destruct an instance.
     *
     * The original contents of the input stream will be restored.
     */
    virtual ~InputFixture();

private:
    std::streambuf* origin;
    std::istream& stream;
};


/**
 * Send output to one or two streams.
 */
class TeeBuffer : public std::streambuf {
public:
    /**
     * Default constructor.
     */
    TeeBuffer() = default;

    /**
     * Construct an instance with target streambufs.
     *
     * @param sbuf1
     * @param sbuf2
     */
    explicit TeeBuffer(std::streambuf* strbuf1, std::streambuf* sbuf2 = nullptr);

    /**
     * Put a character into the output sequence.
     *
     * @param c output character
     * @return output character or EOF
     */
    int overflow(int c) override;

private:
    static constexpr auto eof{traits_type::eof()};
    std::streambuf* sbuf1{nullptr};
    std::streambuf* sbuf2{nullptr};
};


/**
 * Capture stream output for testing.
 */
class OutputFixture {
public:
    /**
     * Construct a new instance.
     *
     * @param stream captured stream
     * @param dest destination stream
     * @param passthru pass captured input along to the original stream
     */
    OutputFixture(std::ostream& stream, std::ostream& dest, bool passthru = false);

    /**
     * Destruct an instance.
     *
     * The captured stream will be restored to its original state.
     */
    virtual ~OutputFixture();

private:
    TeeBuffer teebuf;
    std::streambuf* origin;
    std::ostream& stream;
};

}  // testing::fixture


#endif  // GTEST_FIXTURE_STREAM_HPP
