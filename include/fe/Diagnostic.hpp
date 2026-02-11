#pragma once

#include "fe/SrcLocation.hpp"
#include "utils/aliases.hpp"

#define MAX_ERRORS 20

namespace optiz::fe {

    enum class DiagnosticLevel {
        Info,
        Warning,
        Error,
        Fatal,
    };

    struct Diagnostic {
        SrcSpan m_Span;
        als::String m_Message;
        DiagnosticLevel m_Level;

        void Print() const {
            als::OutStream* out;
            als::String label;
            als::String color;

            switch (m_Level) {
                case DiagnosticLevel::Info:
                    label = "Info";
                    color = "";
                    out   = &als::StdOut;
                    break;
                case DiagnosticLevel::Warning:
                    label = "Warning";
                    color = "\033[1;33m";  // bold yellow
                    out   = &als::StdOut;
                    break;
                case DiagnosticLevel::Error:
                    label = "Error";
                    color = "\033[1;31m";  // bold red
                    out   = &als::StdErr;
                    break;
                case DiagnosticLevel::Fatal:
                    label = "Fatal Error";
                    color = "\033[1;41;37m";  // white text on red background
                    out   = &als::StdErr;
                    break;
            }

            als::OutStringStream oss;

            if (m_Span.isSameLocation()) {
                SrcLocation loc = m_Span.start;

                als::PrintLn(
                    oss,
                    loc.m_File, ":", loc.m_Line, ":", loc.m_Column);

            } else {
                als::PrintLn(
                    oss,
                    m_Span.start.m_File, ":",
                    m_Span.start.m_Line, ":", m_Span.start.m_Column, "-",
                    m_Span.end.m_Line, ":", m_Span.end.m_Column);
            }

            als::PrintLn(out, "[", oss, "] ", color, label, "\033[0m", ": ", m_Message);
        }
    };

    class DiagnosticEngine {
        als::Vec<Diagnostic> m_Reports;
        int m_ErrorsOccured = 0;

    public:
        void Report(SrcSpan span, als::String msg, DiagnosticLevel level) {
            Diagnostic diagnostic = { span, msg, level };

            if (level == DiagnosticLevel::Fatal) {
                m_Reports.insert(m_Reports.begin(), diagnostic);
                Dump();

                exit(1);
            }
            m_Reports.push_back(diagnostic);

            if (level == DiagnosticLevel::Error) {
                m_ErrorsOccured += 1;

                if (m_ErrorsOccured >= MAX_ERRORS) {
                    Report(span, "Too many errors, aborting...", DiagnosticLevel::Fatal);
                }
            }
        }

        void Dump() const {
            for (const auto& d : m_Reports) {
                d.Print();
            }
        }

        bool HasReports() const { return !m_Reports.empty(); }

        bool HasErrors() const { return m_ErrorsOccured; }
    };

}  // namespace optiz::fe

#undef MAX_ERRORS
