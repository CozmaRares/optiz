#include "fe/Parser.hpp"

#include <llvm/Support/Casting.h>

#include <memory>
#include <string>

#include "fe/AST.hpp"

#define NO_PRECEDENCE             -1
#define NEUTRAL_PRECEDENCE        0
#define COMPARISON_PRECEDENCE     1
#define ADDITION_PRECEDENCE       2
#define MULTIPLICATION_PRECEDENCE 3

bool isSupportedUnaryOperation(optiz::fe::TokenType operation);
bool isSupportedBinaryOperation(optiz::fe::TokenType operation);
int getPrecedence(optiz::fe::TokenType operation);

namespace optiz::fe {

    Parser::Parser(const std::string& input, const std::string& file, DiagnosticEngine& diagnosticEngine)
        : m_Lexer(input, file, diagnosticEngine), m_DiagnosticEngine(diagnosticEngine) {
        Advance();
    }

    // PROGRAM ::= ( EXPRESSION ';' )*
    std::unique_ptr<GenericASTNode> Parser::ParseProgram() {
        std::vector<std::unique_ptr<GenericASTNode>> expressions;
        while (m_CurrentToken.m_Type != TokenType::EndOfFile) {
            m_PanicModeEnabled = false;

            std::unique_ptr<GenericASTNode> expression = ParseExpression();
            if (llvm::isa<ErrorAST>(expression)) {
                Synchronize();
                continue;
            }

            if (m_CurrentToken.m_Type != TokenType::SemiColon) {
                ReportError(m_CurrentToken.m_StartLocation, "Expected ';'");
                Synchronize();
                continue;
            }

            Advance();
            expressions.push_back(std::move(expression));
        }

        SrcLocation startLocation, endLocation;

        if (expressions.empty()) {
            startLocation = endLocation = m_CurrentToken.m_StartLocation;
        } else {
            startLocation = expressions.front()->GetStartLocation();
            endLocation   = expressions.back()->GetEndLocation();
        }

        return std::make_unique<ProgramAST>(std::move(expressions), startLocation, endLocation);
    }

    // EXPRESSION ::= UNARY BINOPRHS
    std::unique_ptr<GenericASTNode> Parser::ParseExpression() {
        auto lhs = ParseUnary();
        if (!lhs) {
            return std::make_unique<ErrorAST>();
        }

        return ParseBinOpRHS(std::move(lhs), NEUTRAL_PRECEDENCE);
    }

    // UNARY ::= PRIMARY | ('+' | '-') UNARY
    std::unique_ptr<GenericASTNode> Parser::ParseUnary() {
        if (!isSupportedUnaryOperation(m_CurrentToken.m_Type)) {
            return ParsePrimary();
        }

        TokenType op              = m_CurrentToken.m_Type;
        SrcLocation startLocation = m_CurrentToken.m_StartLocation;
        Advance();

        auto rhs = ParseUnary();
        if (!rhs) {
            return std::make_unique<ErrorAST>();
        }

        return std::make_unique<UnaryExprAST>(op, std::move(rhs), startLocation, rhs->GetEndLocation());
    }

    // BINOPRHS ::= ( ('+' | '-' | '*' | '/') UNARY )*
    std::unique_ptr<GenericASTNode> Parser::ParseBinOpRHS(std::unique_ptr<GenericASTNode> lhs, int originalPrecedence) {
        while (true) {
            int precedence = getPrecedence(m_CurrentToken.m_Type);
            if (precedence < originalPrecedence) {
                return lhs;
            }

            TokenType op = m_CurrentToken.m_Type;
            Advance();

            auto rhs = ParseUnary();
            if (!rhs) {
                return std::make_unique<ErrorAST>();
            }

            int nextPrecedence = getPrecedence(m_CurrentToken.m_Type);
            if (precedence < nextPrecedence) {
                rhs = ParseBinOpRHS(std::move(rhs), nextPrecedence);
                if (!rhs) {
                    return std::make_unique<ErrorAST>();
                }
            }

            SrcLocation startLocation = lhs->GetStartLocation();
            SrcLocation endLocation   = rhs->GetEndLocation();

            lhs = std::make_unique<BinaryExprAST>(std::move(lhs), std::move(rhs), op, startLocation, endLocation);
        }

        while (isSupportedBinaryOperation(m_CurrentToken.m_Type)) {
            TokenType op = m_CurrentToken.m_Type;
            Advance();

            auto rhs = ParsePrimary();
            if (!rhs)
                return std::make_unique<ErrorAST>();

            SrcLocation startLocation = lhs->GetStartLocation();
            SrcLocation endLocation   = rhs->GetEndLocation();

            lhs = std::make_unique<BinaryExprAST>(std::move(lhs), std::move(rhs), op, startLocation, endLocation);
        }
        return lhs;
    }

    std::unique_ptr<GenericASTNode> Parser::ParsePrimary() {
        if (m_CurrentToken.m_Type == TokenType::Number) {
            double value              = std::stod(m_CurrentToken.m_Lexeme);
            SrcLocation startLocation = m_CurrentToken.m_StartLocation;
            SrcLocation endLocation   = m_CurrentToken.m_EndLocation;
            Advance();

            auto node = std::make_unique<NumberExprAST>(value, startLocation, endLocation);
            return node;
        }

        if (m_CurrentToken.m_Type == TokenType::LParen) {
            Advance();
            auto expr = ParseExpression();
            if (!expr)
                return std::make_unique<ErrorAST>();

            if (m_CurrentToken.m_Type != TokenType::RParen) {
                ReportError(m_CurrentToken.m_StartLocation, "Expected ')'");
                return std::make_unique<ErrorAST>();
            }
            Advance();
            return expr;
        }

        ReportError(m_CurrentToken.m_StartLocation, "Expected number or '('");
        return std::make_unique<ErrorAST>();
    }

    void Parser::Advance() {
        do {
            m_CurrentToken = m_Lexer.GetNextToken();
        } while (m_CurrentToken.m_Type == TokenType::Error);
    }

    void Parser::Synchronize() {
        while (m_CurrentToken.m_Type != TokenType::EndOfFile) {
            if (m_CurrentToken.m_Type == TokenType::SemiColon) {
                Advance();
                return;
            }
            Advance();
        }
    }

    void Parser::ReportError(SrcLocation loc, std::string msg) {
        if (m_PanicModeEnabled)
            return;

        m_DiagnosticEngine.Report(loc, msg, DiagnosticLevel::Error);
        m_PanicModeEnabled = true;
    }

}  // namespace optiz::fe

bool isSupportedUnaryOperation(optiz::fe::TokenType operation) {
    switch (operation) {
        case optiz::fe::TokenType::Plus:
        case optiz::fe::TokenType::Minus:
            return true;

        default:
            return false;
    }
}

bool isSupportedBinaryOperation(optiz::fe::TokenType operation) {
    switch (operation) {
        case optiz::fe::TokenType::Plus:
        case optiz::fe::TokenType::Minus:
        case optiz::fe::TokenType::Star:
        case optiz::fe::TokenType::Slash:
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
