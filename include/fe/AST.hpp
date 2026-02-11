#pragma once

#include "fe/ASTVisitor.hpp"
#include "fe/Lexer.hpp"
#include "fe/SrcLocation.hpp"
#include "utils/aliases.hpp"

#define SHARED_METHODS(T)                                         \
    void accept(ASTVisitor& v) const override { v.Visit(*this); } \
    static bool classof(const GenericAstNode* n) { return n->GetType() == NodeType::T; }

namespace optiz::fe {

    enum class NodeType {
        ErrorAstNode,
        ProgramAstNode,
        AssignementAstNode,
        BinaryExpressionAstNode,
        UnaryExpressionAstNode,
        PrimaryExpressionAstNode,
        ScopeAstNode,
        IfAstNode,
        WhileAstNode,
        FunctionAstNode,
        StructAstNode,
        AnnotationProfileAstNode,
        ImportAstNode,
        NumberAstNode,
        CharAstNode,
        StringAstNode,
        ArrayAstNode,
        BooleanAstNode,
    };

    class GenericAstNode {
        NodeType m_Type;

    public:
        GenericAstNode(NodeType type) : m_Type(als::Move(type)) {}
        virtual ~GenericAstNode() = default;

        virtual void accept(ASTVisitor& visitor) const = 0;
        NodeType GetType() const { return m_Type; }
    };

    class ErrorAstNode : public GenericAstNode {
        SrcSpan m_Span;

    public:
        ErrorAstNode() = delete;
        ErrorAstNode(SrcSpan span)
            : GenericAstNode(NodeType::ErrorAstNode), m_Span(als::Move(span)) {}
        SHARED_METHODS(ErrorAstNode);

        const SrcSpan& GetSpan() const { return m_Span; }
    };

    class ProgramAstNode : public GenericAstNode {
        als::Vec<als::Box<GenericAstNode>> m_LanguageConstructs;

    public:
        ProgramAstNode(als::Vec<als::Box<GenericAstNode>> languageConstructs)
            : GenericAstNode(NodeType::ProgramAstNode), m_LanguageConstructs(als::Move(languageConstructs)) {}
        SHARED_METHODS(ProgramAstNode);

        const als::Vec<als::Box<GenericAstNode>>& GetLanguageConstructs() const { return m_LanguageConstructs; }
    };

    struct NamedType;
    struct ArrayType;
    struct PointerType;

    using TypeData = als::Sum<NamedType, ArrayType, PointerType>;

    struct NamedType {
        Spanned<als::String> m_Identifier;
    };

    struct ArrayType {
        als::Box<TypeData> m_ElementType;
        als::Optional<Spanned<uint>> m_Size;
    };

    struct PointerType {
        als::Box<TypeData> m_TargetType;
    };

    class AssignementAstNode : public GenericAstNode {
        SrcSpan m_LetSpan;
        als::Optional<SrcSpan> m_MutSpan;
        Spanned<als::String> m_Identifier;
        TypeData m_ValueType;
        als::Box<GenericAstNode> m_AssignedValue;

    public:
        AssignementAstNode(
            SrcSpan letSpan,
            als::Optional<SrcSpan> mutSpan,
            Spanned<als::String> identifier,
            TypeData m_ValueType,
            als::Box<GenericAstNode> assignedValue)
            : GenericAstNode(NodeType::AssignementAstNode),
              m_LetSpan(als::Move(letSpan)),
              m_MutSpan(als::Move(mutSpan)),
              m_Identifier(als::Move(identifier)),
              m_ValueType(als::Move(m_ValueType)),
              m_AssignedValue(als::Move(assignedValue)) {}
        SHARED_METHODS(AssignementAstNode);

        const SrcSpan& GetLetSpan() const { return m_LetSpan; }
        const als::Optional<SrcSpan>& GetMutSpan() const { return m_MutSpan; }
        const Spanned<als::String>& GetIdentifier() const { return m_Identifier; }
        const TypeData& GetTypeDefinition() const { return m_ValueType; }
        const GenericAstNode* GetAssignedValue() const { return m_AssignedValue.get(); }

        inline bool IsMutable() const;
    };

    class BinaryExpressionAstNode : public GenericAstNode {
        als::Box<GenericAstNode> m_LHS;
        als::Box<GenericAstNode> m_RHS;
        Spanned<TokenType> m_Operator;

    public:
        BinaryExpressionAstNode(
            als::Box<GenericAstNode> lhs,
            als::Box<GenericAstNode> rhs,
            Spanned<TokenType> operator_)
            : GenericAstNode(NodeType::BinaryExpressionAstNode),
              m_LHS(als::Move(lhs)),
              m_RHS(als::Move(rhs)),
              m_Operator(als::Move(operator_)) {}
        SHARED_METHODS(BinaryExpressionAstNode);

        const GenericAstNode* GetLHS() const { return m_LHS.get(); }
        const GenericAstNode* GetRHS() const { return m_RHS.get(); }
        const Spanned<TokenType>& GetOperatorType() const { return m_Operator; }
    };

    class UnaryExpressionAstNode : public GenericAstNode {
        Spanned<TokenType> m_Operator;
        als::Box<GenericAstNode> m_Expression;

    public:
        UnaryExpressionAstNode(
            Spanned<TokenType> operator_,
            als::Box<GenericAstNode> expression)
            : GenericAstNode(NodeType::UnaryExpressionAstNode),
              m_Operator(als::Move(operator_)),
              m_Expression(als::Move(expression)) {}
        SHARED_METHODS(UnaryExpressionAstNode);

        const Spanned<TokenType>& GetOperatorType() const { return m_Operator; }
        const GenericAstNode* GetExpression() const { return m_Expression.get(); }
    };

    class PrimaryExpressionAstNode : public GenericAstNode {
    public:
        struct FunctionCallData {
            als::Vec<als::Box<GenericAstNode>> m_Arguments;
        };

        struct ArrayAccessData {
            als::Box<GenericAstNode> m_Index;
        };

        struct MemberAccessData {
            als::String m_Identifier;
        };

        struct AssignmentData {
            als::Box<GenericAstNode> m_AssignedValue;
        };

        using Suffix = als::Sum<
            FunctionCallData,
            ArrayAccessData,
            MemberAccessData,
            AssignmentData>;

    private:
        als::Box<GenericAstNode> m_Expression;
        als::Vec<Suffix> m_Suffixes;

    public:
        PrimaryExpressionAstNode(als::Box<GenericAstNode> expression)
            : GenericAstNode(NodeType::PrimaryExpressionAstNode), m_Expression(als::Move(expression)) {}
        SHARED_METHODS(PrimaryExpressionAstNode);

        const GenericAstNode* GetExpression() const { return m_Expression.get(); }
    };

    struct AnnotationBase {
        Spanned<als::String> m_Identifier;
        als::Vec<Token> m_AnnotationTokens;

        AnnotationBase(
            Spanned<als::String> identifier,
            als::Vec<Token> annotationTokens)
            : m_Identifier(als::Move(identifier)),
              m_AnnotationTokens(als::Move(annotationTokens)) {}
    };

    struct OptizAnnotation {
        als::Vec<AnnotationBase> m_Settings;
    };

    struct ContractAnnotation {
        als::Vec<AnnotationBase> m_Hints;
    };

    struct UseAnnotation {
        als::Vec<Spanned<als::String>> m_Profiles;
    };

    using Annotation = als::Sum<OptizAnnotation, UseAnnotation>;

    class ScopeAstNode : public GenericAstNode {
    private:
        als::Vec<Annotation> m_Annotations;
        als::Optional<ContractAnnotation> m_Contract;
        als::Vec<als::Box<GenericAstNode>> m_Statements;
        als::Box<GenericAstNode> m_ReturnedValue;

    public:
        ScopeAstNode(
            als::Vec<Annotation> annotations,
            als::Optional<ContractAnnotation> contract,
            als::Vec<als::Box<GenericAstNode>> statements,
            als::Box<GenericAstNode> returnedValue)
            : GenericAstNode(NodeType::ScopeAstNode),
              m_Annotations(als::Move(annotations)),
              m_Contract(als::Move(contract)),
              m_Statements(als::Move(statements)),
              m_ReturnedValue(als::Move(returnedValue)) {}
        SHARED_METHODS(ScopeAstNode);

        const als::Vec<Annotation>& GetAnnotations() const { return m_Annotations; }
        const als::Optional<ContractAnnotation>& GetContract() const { return m_Contract; }
        const als::Vec<als::Box<GenericAstNode>>& GetStatements() const { return m_Statements; }
        const GenericAstNode* GetReturnedValue() const { return m_ReturnedValue.get(); }
    };

    class IfAstNode : public GenericAstNode {
        als::Box<GenericAstNode> m_Condition;
        als::Box<GenericAstNode> m_Then;
        als::Box<GenericAstNode> m_Else;

    public:
        IfAstNode(
            als::Box<GenericAstNode> condition,
            als::Box<GenericAstNode> then,
            als::Box<GenericAstNode> else_)
            : GenericAstNode(NodeType::IfAstNode),
              m_Condition(als::Move(condition)),
              m_Then(als::Move(then)),
              m_Else(als::Move(else_)) {}
        SHARED_METHODS(IfAstNode);

        const GenericAstNode* GetCondition() const { return m_Condition.get(); }
        const GenericAstNode* GetThen() const { return m_Then.get(); }
        const GenericAstNode* GetElse() const { return m_Else.get(); }
    };

    class WhileAstNode : public GenericAstNode {
        als::Box<GenericAstNode> m_Condition;
        als::Box<GenericAstNode> m_Body;

    public:
        WhileAstNode(
            als::Box<GenericAstNode> condition,
            als::Box<GenericAstNode> body)
            : GenericAstNode(NodeType::WhileAstNode),
              m_Condition(als::Move(condition)),
              m_Body(als::Move(body)) {}
        SHARED_METHODS(WhileAstNode);

        const GenericAstNode* GetCondition() const { return m_Condition.get(); }
        const GenericAstNode* GetBody() const { return m_Body.get(); }
    };

    class FunctionAstNode : public GenericAstNode {
    public:
        using Param = als::Pair<Spanned<als::String>, TypeData>;

    private:
        Spanned<als::String> m_Identifier;
        als::Vec<Param> m_Params;
        als::Box<TypeAstNode> m_ReturnType;
        als::Box<GenericAstNode> m_Body;

    public:
        FunctionAstNode(
            Spanned<als::String> identifier,
            als::Vec<Param> params,
            als::Box<TypeAstNode> returnType,
            als::Box<GenericAstNode> body)
            : GenericAstNode(NodeType::FunctionAstNode),
              m_Identifier(als::Move(identifier)),
              m_Params(als::Move(params)),
              m_ReturnType(als::Move(returnType)),
              m_Body(als::Move(body)) {}
        SHARED_METHODS(FunctionAstNode);

        const Spanned<als::String>& GetIdentifier() const { return m_Identifier; }
        const als::Vec<Param>& GetParams() const { return m_Params; }
        const TypeAstNode* GetReturnType() const { return m_ReturnType.get(); }
        const GenericAstNode* GetBody() const { return m_Body.get(); }
    };

    class StructAstNode : public GenericAstNode {
    public:
        using Member = als::Pair<Spanned<als::String>, TypeData>;

    private:
        Spanned<als::String> m_Identifier;
        als::Vec<Member> m_Members;

    public:
        StructAstNode(
            Spanned<als::String> identifier,
            als::Vec<Member> members)
            : GenericAstNode(NodeType::StructAstNode),
              m_Identifier(als::Move(identifier)),
              m_Members(als::Move(members)) {}
        SHARED_METHODS(StructAstNode);

        const Spanned<als::String>& GetIdentifier() const { return m_Identifier; }
        const als::Vec<Member>& GetMembers() const { return m_Members; }
    };

    class AnnotationProfileAstNode : public GenericAstNode {
        Spanned<als::String> m_Name;
        als::Vec<AnnotationBase> m_Settings;

    public:
        AnnotationProfileAstNode(
            Spanned<als::String> name,
            als::Vec<AnnotationBase> settings)
            : GenericAstNode(NodeType::AnnotationProfileAstNode),
              m_Name(als::Move(name)),
              m_Settings(als::Move(settings)) {}
        SHARED_METHODS(AnnotationProfileAstNode);

        const Spanned<als::String>& GetName() const { return m_Name; }
        const als::Vec<AnnotationBase>& GetSettings() const { return m_Settings; }
    };

    class ImportAstNode : public GenericAstNode {
        Spanned<als::String> m_Path;

    public:
        ImportAstNode(Spanned<als::String> path)
            : GenericAstNode(NodeType::ImportAstNode), m_Path(als::Move(path)) {}
        SHARED_METHODS(ImportAstNode);

        const Spanned<als::String>& GetPath() const { return m_Path; }
    };

    class NumberAstNode : public GenericAstNode {
        Spanned<double> m_Value;
        bool m_IsInteger;

    public:
        NumberAstNode(als::String literal, SrcSpan span)
            : GenericAstNode(NodeType::NumberAstNode) {
            double value = als::ToDouble(literal);
            m_Value      = { value, span };
        }
        SHARED_METHODS(NumberAstNode);

        const Spanned<double>& GetValue() const { return m_Value; }
        bool IsInteger() const { return m_IsInteger; }
    };

    class CharAstNode : public GenericAstNode {
        Spanned<char> m_Value;

    public:
        CharAstNode(als::String literal, SrcSpan span)
            : GenericAstNode(NodeType::CharAstNode), m_Value{ literal[0], span } {}
        SHARED_METHODS(CharAstNode);

        const Spanned<char>& GetValue() const { return m_Value; }
    };

    class StringAstNode : public GenericAstNode {
        Spanned<als::String> m_Value;

    public:
        StringAstNode(als::String literal, SrcSpan span)
            : GenericAstNode(NodeType::StringAstNode), m_Value{ literal, span } {}
        SHARED_METHODS(StringAstNode);

        const Spanned<als::String>& GetValue() const { return m_Value; }
    };

    class BooleanAstNode : public GenericAstNode {
        Spanned<bool> m_Value;

    public:
        BooleanAstNode(bool value, SrcSpan span)
            : GenericAstNode(NodeType::BooleanAstNode), m_Value{ value, span } {}
        SHARED_METHODS(BooleanAstNode);

        const Spanned<bool>& GetValue() const { return m_Value; }
    };

    class ArrayAstNode : public GenericAstNode {
        als::Vec<GenericAstNode> m_Elements;

    public:
        ArrayAstNode(als::Vec<GenericAstNode> elements)
            : GenericAstNode(NodeType::ArrayAstNode), m_Elements(als::Move(elements)) {}
        SHARED_METHODS(ArrayAstNode);

        const als::Vec<GenericAstNode>& GetElements() const { return m_Elements; }
    };

}  // namespace optiz::fe

#undef SHARED_METHODS
