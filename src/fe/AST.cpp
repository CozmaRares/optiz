#include "fe/AST.hpp"

#define ACCEPT_IMPL(T) \
    void T::accept(ASTVisitor& visitor) const { visitor.Visit(*this); }

#define CLASSOF_IMPL(T) \
    bool T::classof(const GenericASTNode* n) { return n->GetKind() == NodeKind::T; }

namespace optiz::fe {

    GenericASTNode::GenericASTNode(NodeKind kind, SrcLocation startLocation, SrcLocation endLocation)
        : m_Kind(kind), m_StartLocation(startLocation), m_EndLocation(endLocation) {}

    ErrorAST::ErrorAST() : GenericASTNode(NodeKind::ErrorAST, SrcLocation(), SrcLocation()) {}

    NumberExprAST::NumberExprAST(int value, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(NodeKind::NumberExprAST, startLocation, endLocation), m_Value(value) {}

    UnaryExprAST::UnaryExprAST(TokenType operation, std::unique_ptr<GenericASTNode> expr, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(NodeKind::UnaryExprAST, startLocation, endLocation), m_Operation(operation), m_Expression(std::move(expr)) {}

    BinaryExprAST::BinaryExprAST(std::unique_ptr<GenericASTNode> left, std::unique_ptr<GenericASTNode> right, TokenType operation,
                                 SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(NodeKind::BinaryExprAST, startLocation, endLocation), m_LHS(std::move(left)), m_RHS(std::move(right)), m_Operation(operation) {}

    ProgramAST::ProgramAST(std::vector<std::unique_ptr<GenericASTNode>> expressions, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(NodeKind::ProgramAST, startLocation, endLocation), m_Expressions(std::move(expressions)) {}

    NodeKind GenericASTNode::GetKind() const {
        return m_Kind;
    }

    const SrcLocation& GenericASTNode::GetStartLocation() const {
        return m_StartLocation;
    }

    const SrcLocation& GenericASTNode::GetEndLocation() const {
        return m_EndLocation;
    }

    int NumberExprAST::GetValue() const {
        return m_Value;
    }

    TokenType UnaryExprAST::getOperation() const {
        return m_Operation;
    }

    const GenericASTNode* UnaryExprAST::GetExpr() const {
        return m_Expression.get();
    }

    const GenericASTNode* BinaryExprAST::GetLHS() const {
        return m_LHS.get();
    }

    const GenericASTNode* BinaryExprAST::GetRHS() const {
        return m_RHS.get();
    }

    TokenType BinaryExprAST::GetOperation() const {
        return m_Operation;
    }

    const std::vector<std::unique_ptr<GenericASTNode>>& ProgramAST::GetExpressions() const {
        return m_Expressions;
    }

    ACCEPT_IMPL(ErrorAST)
    CLASSOF_IMPL(ErrorAST)

    ACCEPT_IMPL(NumberExprAST)
    CLASSOF_IMPL(NumberExprAST)

    ACCEPT_IMPL(UnaryExprAST)
    CLASSOF_IMPL(UnaryExprAST)

    ACCEPT_IMPL(BinaryExprAST)
    CLASSOF_IMPL(BinaryExprAST)

    ACCEPT_IMPL(ProgramAST)
    CLASSOF_IMPL(ProgramAST)

}  // namespace optiz::fe

#undef ACCEPT_IMPL
#undef CLASSOF_IMPL
