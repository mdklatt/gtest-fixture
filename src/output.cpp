#include "gtest-fixture/output.hpp"
#include <fstream>
#include <stdexcept>

using std::invalid_argument;
using std::make_unique;
using std::ofstream;
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


OutputFixture::OutputFixture(ostream& stream, ostream& dest, bool passthru):
    teebuf{dest.rdbuf(), passthru ? stream.rdbuf() : nullptr},
    stream{stream},
    origin{stream.rdbuf(&teebuf)} {
    origin->pubsync();
}


OutputFixture::~OutputFixture() {
    stream.rdbuf(origin);
}
