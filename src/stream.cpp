#include "gtest-infra/stream.hpp"
#include <fstream>
#include <stdexcept>

using namespace testing::infra::stream;
using std::invalid_argument;
using std::istream;
using std::make_unique;
using std::ofstream;
using std::ostream;
using std::streambuf;
using std::string;


InputStream::InputStream(istream& stream, std::istream& input):
        origin{stream.rdbuf(input.rdbuf())},
        stream{stream} {}


InputStream::~InputStream() {
    stream.rdbuf(origin);
}


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


OutputStream::OutputStream(ostream& stream, ostream& dest, bool passthru):
    teebuf{dest.rdbuf(), passthru ? stream.rdbuf() : nullptr},
    stream{stream},
    origin{stream.rdbuf(&teebuf)} {
    origin->pubsync();
}


OutputStream::~OutputStream() {
    stream.rdbuf(origin);
}
