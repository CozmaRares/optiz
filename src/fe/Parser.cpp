#include "fe/Parser.hpp"

#include <cassert>

#include "fe/AST.hpp"
#include "fe/SrcLocation.hpp"
#include "utils/aliases.hpp"

#define NO_PRECEDENCE             -1
#define NEUTRAL_PRECEDENCE        0
#define COMPARISON_PRECEDENCE     1
#define ADDITION_PRECEDENCE       2
#define MULTIPLICATION_PRECEDENCE 3

namespace optiz::fe {

    bool isSupportedUnaryOperation(TokenType operation);
    int getPrecedence(TokenType operation);

    Parser::Parser(const als::String& input, const als::String& file, DiagnosticEngine& diagnosticEngine)
        : m_Lexer(input, file, diagnosticEngine), m_DiagnosticEngine(diagnosticEngine) {
        Advance();
    }

    // PROGRAM ::= LANGUAGE_CONSTRUCT*
    als::Box<GenericAstNode> Parser::ParseProgram() {
        als::Vec<als::Box<GenericAstNode>> languageConstructs;
        while (m_CurrentToken.m_Type != TokenType::EndOfFile) {
            m_PanicModeEnabled = false;

            auto languageConstruct = ParseLanguageConstruct();
            if (als::IsA<ErrorAstNode>(languageConstruct)) {
                SyncTo({});
                continue;
            }
        }

        return als::New<ProgramAstNode>(als::Move(languageConstructs));
    }

    // LANGUAGE_CONSTRUCT ::= FUNCTION | STRUCT | ANNOTATION_PROFILE | IMPORT | STATEMENT
    als::Box<GenericAstNode> Parser::ParseLanguageConstruct() {
        if (Match(TokenType::Fn)) {
            return ParseFunction();
        }

        if (Match(TokenType::Struct)) {
            return ParseStruct();
        }

        if (Match(TokenType::AtProfile)) {
            return ParseAnnotationProfile();
        }

        if (Match(TokenType::Import)) {
            return ParseImport();
        }

        return ParseStatement();
    }

    // FUNCTION ::= 'fn' <identifier> '(' PARAMS? ')' TYPE SCOPE
    als::Box<GenericAstNode> Parser::ParseFunction() {
        if (!Match(TokenType::Fn)) {
            return ReportError("Expected 'fn'");
        }
        Advance();

        if (!Match(TokenType::Identifier)) {
            return ReportError("Expected identifier");
        }

        Spanned<als::String> identifier = { m_CurrentToken.m_Lexeme, m_CurrentToken.m_Span };
        Advance();

        if (!Match(TokenType::LParen)) {
            return ReportError("Expected '('");
        }

        Advance();

        als::Box<GenericAstNode> params;
        if (!Match(TokenType::RParen)) {
            params = ParseFunctionParams();
            if (als::IsA<ErrorAstNode>(params)) {
                return params;
            }
        }

        if (!Match(TokenType::RParen)) {
            return ReportError("Expected ')'");
        }
        Advance();

        als::Box<GenericAstNode> returnType = ParseType();
        if (als::IsA<ErrorAstNode>(returnType)) {
            return als::Move(returnType);
        }

        als::Box<GenericAstNode> body = ParseScope();
        if (als::IsA<ErrorAstNode>(body)) {
            return als::Move(body);
        }

        return als::New<FunctionAstNode>(
            identifier,
            params != nullptr ? als::Cast<FunctionParamsAstNode>(als::Move(params)) : nullptr,
            als::Cast<TypeAstNode>(als::Move(returnType)),
            als::Move(body));
    }

    als::Box<GenericAstNode> Parser::ParseFunctionParams() {
        als::Vec<FunctionParamsAstNode::Param> m_Params;

        while (true) {
            if (!Match(TokenType::Identifier)) {
                return ReportError("Expected identifier");
            }

            Spanned<als::String> identifier = { m_CurrentToken.m_Lexeme, m_CurrentToken.m_Span };
            Advance();

            als::Box<GenericAstNode> genericTypeNode = ParseTypeBase();
            if (als::IsA<ErrorAstNode>(genericTypeNode)) {
                return als::Move(genericTypeNode);
            }
            als::Box<TypeAstNode> typeNode = als::Cast<TypeAstNode>(als::Move(genericTypeNode));
            TypeAstNode::TypeData typeData = typeNode->MoveData();

            m_Params.push_back({ identifier, als::Move(typeData) });

            if (!Match(TokenType::Comma)) {
                break;
            }
            Advance();
        }

        return als::New<FunctionParamsAstNode>(als::Move(m_Params));
    }

    als::Box<GenericAstNode> Parser::ParseType() {
        if (!Match(TokenType::Colon)) {
            return ReportError("Expected ':'");
        }
        Advance();

        return ParseTypeBase();
    }

    als::Box<GenericAstNode> Parser::ParseTypeBase() {
        if (Match(TokenType::Identifier)) {
            TypeAstNode::NamedType namedType = { m_CurrentToken.m_Lexeme, m_CurrentToken.m_Span };
            Advance();
            return als::New<TypeAstNode>(namedType);
        }

        if (Match(TokenType::Star)) {
            Advance();

            als::Box<GenericAstNode> genericTypeNode = ParseTypeBase();
            if (als::IsA<ErrorAstNode>(genericTypeNode)) {
                return als::Move(genericTypeNode);
            }
            als::Box<TypeAstNode> typeNode = als::Cast<TypeAstNode>(als::Move(genericTypeNode));
            TypeAstNode::TypeData typeData = typeNode->MoveData();

            TypeAstNode::PointerType pointerType = { als::New<TypeAstNode::TypeData>(typeData) };
            return als::New<TypeAstNode>(pointerType);
        }

        if (Match(TokenType::LParen)) {
            Advance();

            als::Box<GenericAstNode> genericTypeNode = ParseTypeBase();
            if (als::IsA<ErrorAstNode>(genericTypeNode)) {
                return als::Move(genericTypeNode);
            }
            als::Box<TypeAstNode> typeNode = als::Cast<TypeAstNode>(als::Move(genericTypeNode));
            TypeAstNode::TypeData typeData = typeNode->MoveData();

            als::Optional<Spanned<uint>> size = als::None;

            if (Match(TokenType::SemiColon)) {
                Advance();
                if (!Match(TokenType::Number)) {
                    return ReportError("Expected number");
                }

                size = { als::ToUint(m_CurrentToken.m_Lexeme), m_CurrentToken.m_Span };
                Advance();
            }

            if (!Match(TokenType::RParen)) {
                return ReportError("Expected ')'");
            }
            Advance();

            TypeAstNode::ArrayType arrayType = { als::New<TypeAstNode::TypeData>(typeData), size };
            return als::New<TypeAstNode>(arrayType);
        }

        return ReportError("Expected <identifier> or '*' or '('");
    }

    als::Box<GenericAstNode> Parser::ParseScope() {
    }

    // // EXPRESSION ::= UNARY BINOPRHS
    // als::Box<GenericAstNode> Parser::ParseExpression() {
    //     auto lhs = ParseUnary();
    //     if (!lhs) {
    //         return als::New<ErrorAstNode>();
    //     }
    //
    //     return ParseBinOpRHS(als::Move(lhs), NEUTRAL_PRECEDENCE);
    // }
    //
    // // UNARY ::= PRIMARY | ('+' | '-') UNARY
    // als::Box<GenericAstNode> Parser::ParseUnary() {
    //     if (!isSupportedUnaryOperation(m_CurrentToken.m_Type)) {
    //         return ParsePrimary();
    //     }
    //
    //     TokenType op              = m_CurrentToken.m_Type;
    //     SrcLocation startLocation = m_CurrentToken.m_StartLocation;
    //     Advance();
    //
    //     auto rhs = ParseUnary();
    //     if (!rhs) {
    //         return als::New<ErrorAstNode>();
    //     }
    //
    //     return als::New<UnaryExprAST>(op, als::Move(rhs), startLocation, rhs->GetEndLocation());
    // }
    //
    // // BINOPRHS ::= ( ('+' | '-' | '*' | '/') UNARY )*
    // als::Box<GenericAstNode> Parser::ParseBinOpRHS(als::Box<GenericAstNode> lhs, int originalPrecedence) {
    //     while (true) {
    //         int precedence = getPrecedence(m_CurrentToken.m_Type);
    //         if (precedence < originalPrecedence) {
    //             return lhs;
    //         }
    //
    //         TokenType op = m_CurrentToken.m_Type;
    //         Advance();
    //
    //         auto rhs = ParseUnary();
    //         if (!rhs) {
    //             return als::New<ErrorAstNode>();
    //         }
    //
    //         int nextPrecedence = getPrecedence(m_CurrentToken.m_Type);
    //         if (precedence < nextPrecedence) {
    //             rhs = ParseBinOpRHS(als::Move(rhs), nextPrecedence);
    //             if (!rhs) {
    //                 return als::New<ErrorAstNode>();
    //             }
    //         }
    //
    //         SrcLocation startLocation = lhs->GetStartLocation();
    //         SrcLocation endLocation   = rhs->GetEndLocation();
    //
    //         lhs = als::New<BinaryExprAST>(als::Move(lhs), als::Move(rhs), op, startLocation, endLocation);
    //     }
    //
    //     return lhs;
    // }
    //
    // als::Box<GenericAstNode> Parser::ParsePrimary() {
    //     if (m_CurrentToken.m_Type == TokenType::Number) {
    //         double value              = stod(m_CurrentToken.m_Lexeme);
    //         SrcLocation startLocation = m_CurrentToken.m_StartLocation;
    //         SrcLocation endLocation   = m_CurrentToken.m_EndLocation;
    //         Advance();
    //
    //         auto node = als::New<NumberAstNode>(value, startLocation, endLocation);
    //         return node;
    //     }
    //
    //     if (m_CurrentToken.m_Type == TokenType::LParen) {
    //         Advance();
    //         auto expr = ParseExpression();
    //         if (!expr)
    //             return als::New<ErrorAstNode>();
    //
    //         if (m_CurrentToken.m_Type != TokenType::RParen) {
    //             ReportError(m_CurrentToken.m_StartLocation, "Expected ')'");
    //             return als::New<ErrorAstNode>();
    //         }
    //         Advance();
    //         return expr;
    //     }
    //
    //     ReportError(m_CurrentToken.m_StartLocation, "Expected number or '('");
    //     return als::New<ErrorAstNode>();
    // }

    void Parser::Advance() {
        while (true) {
            m_CurrentToken = m_Lexer.GetNextToken();

            if (!IsError()) {
                break;
            }

            m_PanicModeEnabled = true;
        }
    }

    void Parser::SyncTo(als::ListConstructor<TokenType> recoveryTokens) {
        assert(recoveryTokens.size() > 0 && "SyncTo called with empty list of recovery tokens");

        Advance();
        while (!IsAtEnd()) {
            for (auto t : recoveryTokens) {
                if (Match(t))
                    return;
            }
            Advance();
        }
    }

    als::Box<GenericAstNode> Parser::ReportError(als::String msg) {
        SrcSpan span = m_CurrentToken.m_Span;

        if (!m_PanicModeEnabled) {
            m_DiagnosticEngine.Report(span, msg, DiagnosticLevel::Error);
            m_PanicModeEnabled = true;
        }

        return als::New<ErrorAstNode>(m_CurrentToken.m_Span);
    }

    bool isSupportedUnaryOperation(optiz::fe::TokenType operation) {
        switch (operation) {
            case optiz::fe::TokenType::Plus:
            case optiz::fe::TokenType::Minus:
                return true;

            default:
                return false;
        }
    }

    inline int getPrecedence(optiz::fe::TokenType operation) {
        using namespace optiz::fe;

        switch (operation) {
            case TokenType::Plus:
            case TokenType::Minus:
                return ADDITION_PRECEDENCE;

            case TokenType::Star:
            case TokenType::Slash:
                return MULTIPLICATION_PRECEDENCE;

            default:
                return NO_PRECEDENCE;
        }
    }

}  // namespace optiz::fe
