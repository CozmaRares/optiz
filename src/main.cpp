#include "fe/AST.hpp"
#include "fe/ASTPrinter.hpp"
#include "fe/Diagnostic.hpp"
#include "fe/Parser.hpp"

using namespace optiz::fe;

int main() {
    DiagnosticEngine TheDiagnosticEngine;
    Parser parser("* 1 + 2 * 3 * 4;", "main.optiz", TheDiagnosticEngine);
    std::unique_ptr<GenericASTNode> ast = parser.ParseProgram();

    ASTPrinter printer;
    ast->accept(printer);

    if (TheDiagnosticEngine.HasReports()) {
        TheDiagnosticEngine.Dump();

        if (TheDiagnosticEngine.HasErrors()) {
            return 1;
        }
    }

    return 0;
}
