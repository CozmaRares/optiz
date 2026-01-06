#ifndef PARSER_HPP
#define PARSER_HPP

#include "fe/AST.hpp"
#include "fe/Diagnostic.hpp"
#include "fe/Lexer.hpp"

namespace optiz::fe {

    class Parser {
        DiagnosticEngine& diagnosticEngine;
        Lexer lexer;
        Token currentToken;
        bool panicMode;

    public:
        Parser(const std::string& input, const std::string& file, DiagnosticEngine& diagnosticEngine);
        std::unique_ptr<GenericASTNode> parseProgram();

    private:
        std::unique_ptr<GenericASTNode> parseExpression();
        std::unique_ptr<GenericASTNode> parseUnary();
        std::unique_ptr<GenericASTNode> parseBinOpRHS(std::unique_ptr<GenericASTNode> lhs, int operatorPrecedence);
        std::unique_ptr<GenericASTNode> parsePrimary();

        void advance();
        void synchronize();
        void reportError(SrcLocation loc, std::string msg);
    };

}  // namespace optiz::fe

#endif  // PARSER_HPP
