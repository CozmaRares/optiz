#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/NoFolder.h>

#include <expected>
#include <map>
#include <memory>
#include <string>

#include "ast.hpp"

class CodegenModule;

class CodegenSession {
    llvm::LLVMContext context;
    std::map<std::string, std::unique_ptr<CodegenModule>> modules;

public:
    CodegenModule& GetOrCreateModule(const std::string& name);
    CodegenModule* FindModule(const std::string& name) const;
};

class CodegenModule {
    friend class CodegenSession;

    CodegenSession& session;
    std::map<std::string, llvm::Function*> exports;

    CodegenModule(CodegenSession& session, llvm::LLVMContext& context, const std::string& name)
        : session(session), module(name, context), builder(context) {}

public:
    llvm::Module module;
    llvm::IRBuilder<llvm::NoFolder> builder;

    std::expected<llvm::Function*, std::string> GenerateMain(const ast::Expression& expression);
    std::expected<void, std::string> WriteIR(const std::string& path) const;

    std::expected<llvm::Function*, std::string> ExportFunction(const std::string& name, llvm::FunctionType* type);
    std::expected<llvm::Function*, std::string> ImportFunction(const std::string& moduleName, const std::string& name);
};

class CodegenVisitor final : public ast::Visitor {
    llvm::Value* value = nullptr;
    CodegenModule& codegenModule;

public:
    explicit CodegenVisitor(CodegenModule& codegenModule) : codegenModule(codegenModule) {}

    llvm::Value* Generate(const ast::Expression& expression);
    void Visit(const ast::Integer& expression) override;
    void Visit(const ast::Float& expression) override;
    void Visit(const ast::Unary& expression) override;
    void Visit(const ast::Binary& expression) override;
};
