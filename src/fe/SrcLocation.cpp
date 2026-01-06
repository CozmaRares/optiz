#include "fe/SrcLocation.hpp"

namespace optiz::fe {

    void SrcLocation::advance(char current) {
        if (current == '\n') {
            this->line++;
            this->column = 0;
        } else {
            this->column++;
        }
    }

}  // namespace optiz::fe
