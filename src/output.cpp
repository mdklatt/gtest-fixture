#include "gtest-fixture/output.hpp"
#include <stdexcept>

using std::invalid_argument;
using std::ostream;
using std::streambuf;
using std::string;
using testing::fixture::OutputFixture;
using testing::fixture::TeeBuffer;


TeeBuffer::TeeBuffer(streambuf* strbuf1, streambuf* sbuf2):
    sbuf1{strbuf1},
    sbuf2{sbuf2} {}


int TeeBuffer::overflow(int c) {
    // Trading performance for ease of implementation by foregoing any
    // buffering.
    const auto c1{sbuf1 ? sbuf1->sputc(c) : c};
    const auto c2{sbuf2 ? sbuf2->sputc(c) : c};
    return c1 == eof or c2 == eof ? eof : c;
}


OutputFixture::OutputFixture(ostream& stream, bool passthru) :
    origin{stream.rdbuf()},
    stream{stream} {
    origin->pubsync();
    auto strbuf2{passthru ? origin : nullptr};
    teebuf = TeeBuffer{target.rdbuf(), strbuf2};
    stream.rdbuf(&teebuf);
}


OutputFixture::~OutputFixture() {
    stream.rdbuf(origin);
}


string OutputFixture::str() const {
    return target.str();
}
