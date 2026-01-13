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
        std::unique_ptr<GenericASTNode> ParseStatement();
        std::unique_ptr<GenericASTNode> ParseExpression();
        std::unique_ptr<GenericASTNode> ParseBinaryExpression();
        std::unique_ptr<GenericASTNode> ParseUnaryExpression();
        std::unique_ptr<GenericASTNode> ParsePrimaryExpression();
        std::unique_ptr<void> ParseSuffix();
        std::unique_ptr<GenericASTNode> ParseAnnotation();
        std::unique_ptr<void> ParseAnnotationMulti();
        std::unique_ptr<void> ParseAnnotationRepeated();
        std::unique_ptr<void> ParseAnnotationBase();
        std::unique_ptr<void> ParseAnnotationUse();
        std::unique_ptr<GenericASTNode> ParseScope();
        std::unique_ptr<GenericASTNode> ParseTypeDefinition();
        std::unique_ptr<void> ParseType();
        std::unique_ptr<GenericASTNode> ParseIf();
        std::unique_ptr<GenericASTNode> ParseWhile();
        std::unique_ptr<GenericASTNode> ParseFunction();
        std::unique_ptr<GenericASTNode> ParseStruct();
        std::unique_ptr<GenericASTNode> ParseAnnotationDef();

        // std::unique_ptr<GenericASTNode> ParseExpression();
        // std::unique_ptr<GenericASTNode> ParseUnary();
        // std::unique_ptr<GenericASTNode> ParseBinOpRHS(std::unique_ptr<GenericASTNode> lhs, int operatorPrecedence);
        // std::unique_ptr<GenericASTNode> ParsePrimary();

        void Advance();
        void Synchronize();
        void ReportError(SrcLocation loc, std::string msg);
    };

}  // namespace optiz::fe
