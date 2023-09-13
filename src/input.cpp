#include "gtest-fixture/input.hpp"
#include <fstream>

using std::ifstream;
using std::istream;
using std::istringstream;
using std::make_unique;
using std::streambuf;
using std::string;
using testing::fixture::InputFixture;


InputFixture::InputFixture(istream& stream, std::istream& input):
    origin{stream.rdbuf(input.rdbuf())},
    stream{stream} {}


InputFixture::~InputFixture() {
    stream.rdbuf(origin);
}
