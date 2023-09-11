/**
 * Capture output streams for inspection.
 */
#ifndef GTEST_FIXTURE_OUTPUT_HPP
#define GTEST_FIXTURE_OUTPUT_HPP

#include "shared.hpp"
#include <filesystem>
#include <memory>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>
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

    /** @overload */
    InputFixture(std::istream& stream, const std::string& input);

    /** @overload */
    InputFixture(std::istream& stream, const std::filesystem::path& input);

    /**
     * Destruct an instance.
     *
     * The original contents of the input stream will be restored.
     */
    virtual ~InputFixture();

private:
    std::unique_ptr<std::istream> buffer;
    std::streambuf* origin;
    std::istream& stream;
};

}  // testing::fixture


#endif  // GTEST_FIXTURE_OUTPUT_HPP
