#pragma once

#include <iosfwd>

#include "ast.hpp"

class AstPrinter final : public ast::Visitor {
    std::ostream& outputStream;

public:
    explicit AstPrinter(std::ostream& outputStream) : outputStream(outputStream) {}

    void Print(const ast::Expression& expression);
    void Visit(const ast::Integer& expression) override;
    void Visit(const ast::Float& expression) override;
    void Visit(const ast::Unary& expression) override;
    void Visit(const ast::Binary& expression) override;
};
