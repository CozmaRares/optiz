#pragma once

#include <ostream>
#include <string>

namespace optiz::fe {

    struct SrcLocation {
        uint m_Line;
        uint m_Column;
        std::string m_File;

        friend std::ostream& operator<<(std::ostream& out, const SrcLocation& loc);
        friend class Lexer;

    private:
        void advance(const char current);
    };

}  // namespace optiz::fe
