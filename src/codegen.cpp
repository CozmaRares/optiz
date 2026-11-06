#include "codegen.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include <cassert>

CodegenModule& CodegenSession::GetOrCreateModule(const std::string& name) {
    auto& codegen = modules[name];
    if (!codegen) codegen.reset(new CodegenModule(*this, context, name));
    return *codegen;
}

CodegenModule* CodegenSession::FindModule(const std::string& name) const {
    auto it = modules.find(name);
    return it == modules.end() ? nullptr : it->second.get();
}

std::expected<llvm::Function*, std::string> CodegenModule::ExportFunction(const std::string& name, llvm::FunctionType* type) {
    if (&type->getContext() != &module.getContext()) return std::unexpected("Function type belongs to another compilation session");
    if (auto it = exports.find(name); it != exports.end()) {
        if (it->second->getFunctionType() != type) return std::unexpected("Conflicting export: " + name);
        return it->second;
    }

    const auto owner  = module.getModuleIdentifier();
    const auto symbol = "_O" + owner + std::to_string(name.size()) + name;
    if (module.getNamedValue(symbol)) return std::unexpected("Symbol already exists: " + symbol);

    auto* function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, symbol, module);
    exports.emplace(name, function);
    return function;
}

std::expected<llvm::Function*, std::string> CodegenModule::ImportFunction(const std::string& moduleName, const std::string& name) {
    auto* source = session.FindModule(moduleName);
    if (!source) return std::unexpected("Unknown module: " + moduleName);

    auto it = source->exports.find(name);
    if (it == source->exports.end()) return std::unexpected("Unknown export: " + moduleName + "." + name);

    auto* exported = it->second;
    if (auto* existing = module.getNamedValue(exported->getName())) {
        auto* function = llvm::dyn_cast<llvm::Function>(existing);
        if (!function || function->getFunctionType() != exported->getFunctionType()) return std::unexpected("Conflicting import: " + name);
        return function;
    }

    return llvm::Function::Create(exported->getFunctionType(), llvm::Function::ExternalLinkage, exported->getName(), module);
}

std::expected<llvm::Function*, std::string> CodegenModule::GenerateMain(const ast::Expression& expression) {
    if (module.getNamedValue("main")) return std::unexpected("Entry point already exists in module: " + module.getModuleIdentifier());

    CodegenVisitor visitor(*this);
    llvm::IRBuilderBase::InsertPointGuard insertionPoint(builder);
    auto* type     = llvm::FunctionType::get(builder.getInt32Ty(), false);
    auto* function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, "main", module);
    builder.SetInsertPoint(llvm::BasicBlock::Create(module.getContext(), "entry", function));

    auto* result = visitor.Generate(expression);

    // main returns a process exit value, independent of the expression's type.
    if (result->getType()->isDoubleTy()) {
        result = builder.CreateFPToSI(result, builder.getInt32Ty());
    } else {
        result = builder.CreateIntCast(result, builder.getInt32Ty(), true);
    }
    builder.CreateRet(result);

    std::string error;
    llvm::raw_string_ostream diagnostics(error);
    if (llvm::verifyFunction(*function, &diagnostics)) {
        builder.ClearInsertionPoint();
        function->eraseFromParent();
        return std::unexpected("Invalid generated entry point: " + error);
    }
    return function;
}

std::expected<void, std::string> CodegenModule::WriteIR(const std::string& path) const {
    std::string error;
    llvm::raw_string_ostream diagnostics(error);
    if (llvm::verifyModule(module, &diagnostics)) return std::unexpected("Invalid module: " + error);

    std::error_code fileError;
    llvm::raw_fd_ostream output(path, fileError);
    if (fileError) return std::unexpected("Cannot open " + path + ": " + fileError.message());
    module.print(output, nullptr);
    output.close();
    if (output.has_error()) {
        auto message = output.error().message();
        output.clear_error();
        return std::unexpected("Cannot write " + path + ": " + message);
    }
    return {};
}

llvm::Value* CodegenVisitor::Generate(const ast::Expression& expression) {
    expression.Accept(*this);
    return value;
}

void CodegenVisitor::Visit(const ast::Integer& expression) {
    auto& builder = codegenModule.builder;
    value         = llvm::ConstantInt::getSigned(builder.getInt64Ty(), expression.value);
}

void CodegenVisitor::Visit(const ast::Float& expression) {
    auto& builder = codegenModule.builder;
    value         = llvm::ConstantFP::get(builder.getDoubleTy(), expression.value);
}

void CodegenVisitor::Visit(const ast::Unary& expression) {
    auto& builder        = codegenModule.builder;
    llvm::Value* operand = Generate(expression.Operand());
    switch (expression.op) {
        case ast::UnaryOperator::Plus: value = operand; return;
        case ast::UnaryOperator::Negate:
            value = operand->getType()->isDoubleTy() ? builder.CreateFNeg(operand) : builder.CreateNeg(operand);
            return;
        default: {
            assert(false && "Unknown unary operator");
            __builtin_unreachable();
        }
    }
}

void CodegenVisitor::Visit(const ast::Binary& expression) {
    auto& builder       = codegenModule.builder;
    llvm::Value* left   = Generate(expression.Left());
    llvm::Value* right  = Generate(expression.Right());
    const bool floating = left->getType()->isDoubleTy() || right->getType()->isDoubleTy();
    if (floating) {
        if (left->getType()->isIntegerTy()) left = builder.CreateSIToFP(left, builder.getDoubleTy());
        if (right->getType()->isIntegerTy()) right = builder.CreateSIToFP(right, builder.getDoubleTy());
    }

    switch (expression.op) {
        case ast::BinaryOperator::Add:
            value = floating ? builder.CreateFAdd(left, right) : builder.CreateAdd(left, right);
            return;
        case ast::BinaryOperator::Subtract:
            value = floating ? builder.CreateFSub(left, right) : builder.CreateSub(left, right);
            return;
        case ast::BinaryOperator::Multiply:
            value = floating ? builder.CreateFMul(left, right) : builder.CreateMul(left, right);
            return;
        case ast::BinaryOperator::Divide:
            value = floating ? builder.CreateFDiv(left, right) : builder.CreateSDiv(left, right);
            return;
        case ast::BinaryOperator::Remainder:
            value = floating ? builder.CreateFRem(left, right) : builder.CreateSRem(left, right);
            return;
        default: {
            assert(false && "Unknown binary operator");
            __builtin_unreachable();
        }
    }
}
