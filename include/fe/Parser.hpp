#pragma once

#include "fe/AST.hpp"
#include "fe/Diagnostic.hpp"
#include "fe/Lexer.hpp"

namespace optiz::fe {

    class Parser {
        DiagnosticEngine& m_DiagnosticEngine;
        Lexer m_Lexer;
        Token m_CurrentToken;
        bool m_PanicModeEnabled;

    public:
        Parser(const std::string& input, const std::string& file, DiagnosticEngine& diagnosticEngine);
        std::unique_ptr<GenericASTNode> ParseProgram();

    private:
        std::unique_ptr<GenericASTNode> ParseExpression();
        std::unique_ptr<GenericASTNode> ParseUnary();
        std::unique_ptr<GenericASTNode> ParseBinOpRHS(std::unique_ptr<GenericASTNode> lhs, int operatorPrecedence);
        std::unique_ptr<GenericASTNode> ParsePrimary();

        void Advance();
        void Synchronize();
        void ReportError(SrcLocation loc, std::string msg);
    };

}  // namespace optiz::fe
