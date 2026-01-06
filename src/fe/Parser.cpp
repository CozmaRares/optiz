#include "fe/Parser.hpp"

#include <memory>
#include <string>

#include "fe/AST.hpp"
#include "fe/ASTPrinter.hpp"

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
        : lexer(input, file, diagnosticEngine), diagnosticEngine(diagnosticEngine) {
        this->advance();
    }

    // PROGRAM ::= ( EXPRESSION ';' )*
    std::unique_ptr<GenericASTNode> Parser::parseProgram() {
        std::vector<std::unique_ptr<GenericASTNode>> expressions;
        while (this->currentToken.type != TokenType::EndOfFile) {
            this->panicMode = false;

            std::unique_ptr<GenericASTNode> expression = this->parseExpression();
            if (!expression) {
                synchronize();
                continue;
            }

            if (this->currentToken.type != TokenType::SemiColon) {
                this->reportError(this->currentToken.location.first, "Expected ';'");
                synchronize();
                continue;
            }

            advance();
            expressions.push_back(std::move(expression));
        }

        SrcLocation startLocation, endLocation;

        if (expressions.empty()) {
            startLocation = endLocation = this->currentToken.location.first;
        } else {
            startLocation = expressions.front()->startLocation;
            endLocation   = expressions.back()->endLocation;
        }

        return std::make_unique<ProgramAST>(std::move(expressions), startLocation, endLocation);
    }

    // EXPRESSION ::= UNARY BINOPRHS
    std::unique_ptr<GenericASTNode> Parser::parseExpression() {
        auto lhs = this->parseUnary();
        if (!lhs) {
            return nullptr;
        }

        return this->parseBinOpRHS(std::move(lhs), NEUTRAL_PRECEDENCE);
    }

    // UNARY ::= PRIMARY | ('+' | '-') UNARY
    std::unique_ptr<GenericASTNode> Parser::parseUnary() {
        if (!isSupportedUnaryOperation(this->currentToken.type)) {
            return this->parsePrimary();
        }

        TokenType op              = this->currentToken.type;
        SrcLocation startLocation = this->currentToken.location.first;
        advance();

        auto rhs = this->parseUnary();
        if (!rhs) {
            return nullptr;
        }

        return std::make_unique<UnaryExprAST>(op, std::move(rhs), startLocation, rhs->endLocation);
    }

    // BINOPRHS ::= ( ('+' | '-' | '*' | '/') UNARY )*
    std::unique_ptr<GenericASTNode> Parser::parseBinOpRHS(std::unique_ptr<GenericASTNode> lhs, int originalPrecedence) {
        while (true) {
            int precedence = getPrecedence(this->currentToken.type);
            if (precedence < originalPrecedence) {
                return lhs;
            }

            TokenType op = this->currentToken.type;
            advance();

            auto rhs = this->parseUnary();
            if (!rhs) {
                return nullptr;
            }

            int nextPrecedence = getPrecedence(this->currentToken.type);
            if (precedence < nextPrecedence) {
                rhs = this->parseBinOpRHS(std::move(rhs), nextPrecedence);
                if (!rhs) {
                    return nullptr;
                }
            }

            SrcLocation startLocation = lhs->startLocation;
            SrcLocation endLocation   = rhs->endLocation;

            lhs = std::make_unique<BinaryExprAST>(std::move(lhs), std::move(rhs), op, startLocation, endLocation);
        }

        while (isSupportedBinaryOperation(this->currentToken.type)) {
            TokenType op = this->currentToken.type;
            advance();

            auto rhs = this->parsePrimary();
            if (!rhs)
                return nullptr;

            SrcLocation startLocation = lhs->startLocation;
            SrcLocation endLocation   = rhs->endLocation;

            lhs = std::make_unique<BinaryExprAST>(std::move(lhs), std::move(rhs), op, startLocation, endLocation);
        }
        return lhs;
    }

    std::unique_ptr<GenericASTNode> Parser::parsePrimary() {
        if (this->currentToken.type == TokenType::Number) {
            double value              = std::stod(this->currentToken.lexeme);
            SrcLocation startLocation = this->currentToken.location.first;
            SrcLocation endLocation   = this->currentToken.location.second;
            advance();

            auto node = std::make_unique<NumberExprAST>(value, startLocation, endLocation);
            return node;
        }

        if (this->currentToken.type == TokenType::LParen) {
            advance();
            auto expr = parseExpression();
            if (!expr)
                return nullptr;

            if (this->currentToken.type != TokenType::RParen) {
                this->reportError(currentToken.location.first, "Expected ')'");
                return nullptr;
            }
            advance();
            return expr;
        }

        this->reportError(currentToken.location.first, "Expected number or '('");
        return nullptr;
    }

    void Parser::advance() {
        do {
            this->currentToken = this->lexer.nextToken();
        } while (this->currentToken.type == TokenType::Error);
    }

    void Parser::synchronize() {
        while (this->currentToken.type != TokenType::EndOfFile) {
            if (this->currentToken.type == TokenType::SemiColon) {
                advance();
                return;
            }
            advance();
        }
    }

    void Parser::reportError(SrcLocation loc, std::string msg) {
        if (panicMode)
            return;

        diagnosticEngine.report(loc, msg, DiagnosticLevel::Error);
        panicMode = true;
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

int getPrecedence(optiz::fe::TokenType operation) {
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
