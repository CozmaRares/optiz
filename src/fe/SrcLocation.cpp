#include "fe/SrcLocation.hpp"

namespace optiz::fe {

    void SrcLocation::advance(char current) {
        if (current == '\n') {
            m_Line++;
            m_Column = 0;
        } else {
            m_Column++;
        }
    }

}  // namespace optiz::fe
