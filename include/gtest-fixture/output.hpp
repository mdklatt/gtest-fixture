/**
 * Capture output streams for inspection.
 */
#ifndef GTEST_FIXTURE_OUTPUT_HPP
#define GTEST_FIXTURE_OUTPUT_HPP

#include <ostream>
#include <sstream>
#include <streambuf>
#include <vector>

namespace testing::fixture {

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
    static constexpr size_t buflen{1024};
    std::vector<char> buffer;
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
     * @param stream stream to capture
     * @param passthru pass captured output along to the original stream
     */
    explicit OutputFixture(std::ostream& stream, bool passthru = false);

    /**
     * Get the captured output.
     *
     * @return
     */
    std::string str() const;

    /**
     * Destruct an instance.
     *
     * The captured stream will be restored to its original state.
     */
    virtual ~OutputFixture();

private:
    TeeBuffer teebuf;
    std::ostringstream target;
    std::streambuf* origin;
    std::ostream& stream;
};

}  // testing::fixture


#endif  // GTEST_FIXTURE_OUTPUT_HPP
