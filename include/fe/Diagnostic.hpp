#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP

#include <vector>

#include "fe/SrcLocation.hpp"

namespace optiz::fe {

    enum class DiagnosticLevel {
        Info    = 0,
        Warning = 1,
        Error   = 2,
        Fatal   = 3
    };

    struct Diagnostic {
        SrcLocation loc;
        std::string message;
        DiagnosticLevel level;

        void print() const;
    };

    class DiagnosticEngine {
        std::vector<Diagnostic> reports;
        bool errorsOccured = false;

    public:
        void report(SrcLocation loc, std::string msg, DiagnosticLevel level);
        void dump() const;
        bool hasReports() const;
        bool hasErrors() const;
    };

}  // namespace optiz::fe

#endif  // DIAGNOSTIC_HPP
