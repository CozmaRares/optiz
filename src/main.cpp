#include "fe/AST.hpp"
#include "fe/ASTPrinter.hpp"
#include "fe/Diagnostic.hpp"
#include "fe/Parser.hpp"

using namespace optiz::fe;

int main() {
    DiagnosticEngine TheDiagnosticEngine;
    Parser parser("1 + 2 * 3 * 4;", "main.optiz", TheDiagnosticEngine);
    std::unique_ptr<GenericASTNode> ast = parser.parseProgram();

    if (TheDiagnosticEngine.hasReports()) {
        TheDiagnosticEngine.dump();

        if (TheDiagnosticEngine.hasErrors()) {
            return 1;
        }
    }

    ASTPrinter printer;
    ast->accept(printer);

    return 0;
}
