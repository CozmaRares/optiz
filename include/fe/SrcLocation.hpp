#ifndef SRCLOCATION_HPP
#define SRCLOCATION_HPP

#include <ostream>
#include <string>

namespace optiz::fe {

    struct SrcLocation {
        uint line;
        uint column;
        std::string file;

        friend std::ostream& operator<<(std::ostream& out, const SrcLocation& loc);
        friend class Lexer;

    private:
        void advance(const char current);
    };

}  // namespace optiz::fe

#endif  // SRCLOCATION_HPP
