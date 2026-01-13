#pragma once

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
        SrcLocation m_Location;
        std::string m_Message;
        DiagnosticLevel m_Level;

        void Print() const;
    };

    class DiagnosticEngine {
        std::vector<Diagnostic> m_Reports;
        bool m_ErrorsOccured = false;

    public:
        void Report(SrcLocation loc, std::string msg, DiagnosticLevel level);
        void Dump() const;
        bool HasReports() const;
        bool HasErrors() const;
    };

}  // namespace optiz::fe
