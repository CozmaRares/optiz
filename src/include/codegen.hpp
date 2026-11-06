#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/NoFolder.h>

class Codegen {
public:
    llvm::LLVMContext context;
    llvm::Module module;
    llvm::IRBuilder<llvm::NoFolder> builder;

    Codegen() : module("optiz", context), builder(context) {}
};
