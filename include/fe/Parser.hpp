#pragma once

#include "fe/AST.hpp"
#include "fe/Diagnostic.hpp"
#include "fe/Lexer.hpp"
#include "utils/aliases.hpp"

namespace optiz::fe {

    class Parser {
        DiagnosticEngine& m_DiagnosticEngine;
        Lexer m_Lexer;
        Token m_CurrentToken;
        bool m_PanicModeEnabled;

    public:
        Parser(const als::String& input, const als::String& file, DiagnosticEngine& diagnosticEngine);
        als::Box<GenericAstNode> ParseProgram();

    private:
        using ParseError = als::Pair<SrcSpan, als::String>;

        template <typename T>
        using ParseResult = als::Result<T, ParseError>;

        als::Box<GenericAstNode> ParseLanguageConstruct();
        als::Box<GenericAstNode> ParseFunction();
        ParseResult<FunctionAstNode::Param> ParseFunctionParams();
        ParseResult<TypeData> ParseType();
        ParseResult<TypeData> ParseTypeBase();
        als::Box<GenericAstNode> ParseScope();
        ParseResult<Annotation> ParseAnnotation();
        ParseResult<Annotation> ParseAnnotationOptiz();
        ParseResult<Annotation> ParseAnnotationUse();
        ParseResult<ContractAnnotation> ParseContract();

        void Advance();
        void SyncTo(als::ListConstructor<TokenType> recoveryTokens);
        als::Box<GenericAstNode> ReportError(als::String msg);

        inline bool Match(TokenType type) { return m_CurrentToken.m_Type == type; }
        inline bool IsAtEnd() { return Match(TokenType::EndOfFile); }
        inline bool IsError() { return Match(TokenType::Error); }
    };

}  // namespace optiz::fe
