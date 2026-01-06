#include "fe/Diagnostic.hpp"

#include <iostream>
#include <ostream>

#define MAX_ERRORS 20

namespace optiz::fe {

    std::ostream& getOutputStream(DiagnosticLevel level) {
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

    void printLabel(DiagnosticLevel level, std::ostream& out) {
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

    void Diagnostic::print() const {
        std::ostream& out = getOutputStream(this->level);

        out << "[" << loc.file << ":" << loc.line << ":" << loc.column << "] ";

        printLabel(this->level, out);

        out << ": " << message << std::endl;
    }

    void DiagnosticEngine::report(SrcLocation loc, std::string msg, DiagnosticLevel level) {
        this->errorsOccured = this->errorsOccured || level >= DiagnosticLevel::Error;

        Diagnostic diagnostic = { loc, msg, level };

        if (level == DiagnosticLevel::Fatal) {
            reports.insert(reports.begin(), diagnostic);
            this->dump();

            exit(1);
        }
        reports.push_back(diagnostic);

        if (reports.size() > MAX_ERRORS) {
            this->report(loc, "Too many errors, aborting...", DiagnosticLevel::Fatal);
        }
    }

    void DiagnosticEngine::dump() const {
        for (const auto& d : reports) {
            d.print();
        }
    }

    bool DiagnosticEngine::hasReports() const {
        return !reports.empty();
    }
    bool DiagnosticEngine::hasErrors() const {
        return this->errorsOccured;
    }

}  // namespace optiz::fe
