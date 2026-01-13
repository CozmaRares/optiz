#include "fe/Diagnostic.hpp"

#include <iostream>
#include <ostream>

#define MAX_ERRORS 20

static std::ostream& getOutputStream(optiz::fe::DiagnosticLevel level);
static void printLabel(optiz::fe::DiagnosticLevel level, std::ostream& out);

namespace optiz::fe {

    void Diagnostic::Print() const {
        std::ostream& out = getOutputStream(m_Level);

        out << "[" << m_Location.m_File << ":" << m_Location.m_Line << ":" << m_Location.m_Column << "] ";

        printLabel(m_Level, out);

        out << ": " << m_Message << std::endl;
    }

    void DiagnosticEngine::Report(SrcLocation loc, std::string msg, DiagnosticLevel level) {
        m_ErrorsOccured = m_ErrorsOccured || level >= DiagnosticLevel::Error;

        Diagnostic diagnostic = { loc, msg, level };

        if (level == DiagnosticLevel::Fatal) {
            m_Reports.insert(m_Reports.begin(), diagnostic);
            Dump();

            exit(1);
        }
        m_Reports.push_back(diagnostic);

        if (m_Reports.size() > MAX_ERRORS) {
            Report(loc, "Too many errors, aborting...", DiagnosticLevel::Fatal);
        }
    }

    void DiagnosticEngine::Dump() const {
        for (const auto& d : m_Reports) {
            d.Print();
        }
    }

    bool DiagnosticEngine::HasReports() const {
        return !m_Reports.empty();
    }
    bool DiagnosticEngine::HasErrors() const {
        return m_ErrorsOccured;
    }

}  // namespace optiz::fe

static std::ostream& getOutputStream(optiz::fe::DiagnosticLevel level) {
    using optiz::fe::DiagnosticLevel;

    switch (level) {
        case DiagnosticLevel::Info:
        case DiagnosticLevel::Warning:
            return std::cout;

        case DiagnosticLevel::Error:
        case DiagnosticLevel::Fatal:
            return std::cerr;
    }

    return std::cout;
}

static void printLabel(optiz::fe::DiagnosticLevel level, std::ostream& out) {
    using optiz::fe::DiagnosticLevel;

    std::string label;
    std::string color;

    switch (level) {
        case DiagnosticLevel::Info:
            label = "Info";
            color = "";
            break;
        case DiagnosticLevel::Warning:
            label = "Warning";
            color = "\033[1;33m";  // Bold Yellow
            break;
        case DiagnosticLevel::Error:
            label = "Error";
            color = "\033[1;31m";  // Bold Red
            break;
        case DiagnosticLevel::Fatal:
            label = "Fatal Error";
            color = "\033[1;41;37m";  // White text on Red background
            break;
    }

    out << color << label << "\033[0m";
}
