#pragma once

namespace optiz::fe {

    class ErrorAstNode;
    class ProgramAstNode;
    class AssignementAstNode;
    class BinaryExpressionAstNode;
    class UnaryExpressionAstNode;
    class PrimaryExpressionAstNode;
    class AnnotationAstNode;
    class ScopeAstNode;
    class IfAstNode;
    class WhileAstNode;
    class FunctionParamsAstNode;
    class FunctionAstNode;
    class StructAstNode;
    class AnnotationProfileAstNode;
    class ImportAstNode;
    class NumberAstNode;
    class CharAstNode;
    class StringAstNode;
    class ArrayAstNode;
    class BooleanAstNode;
    class TypeAstNode;

    class ASTVisitor {
    public:
        virtual ~ASTVisitor() = default;

        virtual void Visit(const ErrorAstNode& node)             = 0;
        virtual void Visit(const ProgramAstNode& node)           = 0;
        virtual void Visit(const AssignementAstNode& node)       = 0;
        virtual void Visit(const BinaryExpressionAstNode& node)  = 0;
        virtual void Visit(const UnaryExpressionAstNode& node)   = 0;
        virtual void Visit(const PrimaryExpressionAstNode& node) = 0;
        virtual void Visit(const AnnotationAstNode& node)        = 0;
        virtual void Visit(const ScopeAstNode& node)             = 0;
        virtual void Visit(const IfAstNode& node)                = 0;
        virtual void Visit(const WhileAstNode& node)             = 0;
        virtual void Visit(const FunctionParamsAstNode& node)    = 0;
        virtual void Visit(const FunctionAstNode& node)          = 0;
        virtual void Visit(const StructAstNode& node)            = 0;
        virtual void Visit(const AnnotationProfileAstNode& node) = 0;
        virtual void Visit(const ImportAstNode& node)            = 0;
        virtual void Visit(const NumberAstNode& node)            = 0;
        virtual void Visit(const CharAstNode& node)              = 0;
        virtual void Visit(const StringAstNode& node)            = 0;
        virtual void Visit(const ArrayAstNode& node)             = 0;
        virtual void Visit(const BooleanAstNode& node)           = 0;
        virtual void Visit(const TypeAstNode& node)              = 0;
    };

}  // namespace optiz::fe
