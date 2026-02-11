#pragma once

#include "utils/aliases.hpp"

namespace optiz::fe {

    struct SrcLocation {
        uint m_Line;
        uint m_Column;
        als::String m_File;

        friend class Lexer;

    private:
        void advance(const char current) {
            if (current == '\n') {
                m_Line++;
                m_Column = 0;
            } else {
                m_Column++;
            }
        }
    };

    struct SrcSpan {
        SrcLocation start;
        SrcLocation end;

        bool isSameLocation() const {
            return start.m_Line == end.m_Line &&
                   start.m_Column == end.m_Column &&
                   start.m_File == end.m_File;
        }
    };

    template <typename T>
    struct Spanned {
        T m_Data;
        SrcSpan m_Span;
    };

    inline als::OutStream& operator<<(als::OutStream& out, const SrcLocation& loc) {
        als::Print(out, "SrcLocation { line = ", loc.m_Line, ", col = ", loc.m_Column, ", file = ", loc.m_File, " }");
        return out;
    }

    inline als::OutStream& operator<<(als::OutStream& out, const SrcSpan& span) {
        als::Print(out, "SrcSpan { start = ", span.start, ", end = ", span.end, " }");
        return out;
    }

}  // namespace optiz::fe
