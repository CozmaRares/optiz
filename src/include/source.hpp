#pragma once

#include <cassert>
#include <cstddef>

struct SourceLocation {
    size_t line   = 1;
    size_t column = 1;
};

struct SourceSpan {
    size_t offset = 0;
    size_t length = 0;
    SourceLocation location;

    size_t End() const { return offset + length; }
};

inline SourceSpan SpanFromTo(SourceSpan first, SourceSpan last) {
    assert(first.offset <= last.End());
    return { first.offset, last.End() - first.offset, first.location };
}
