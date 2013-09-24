#pragma once

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"

#include <map>
#include <string>

using ::llvm::AllocaInst;
using ::llvm::ExecutionEngine;
using ::llvm::Function;
using ::llvm::FunctionPassManager;
using ::llvm::IRBuilder;
using ::llvm::LLVMContext;
using ::llvm::Module;


class Context {
    Module *module_;
    ExecutionEngine *engine_;
    FunctionPassManager *pass_manager_;
    IRBuilder<> *builder_;

    std::map<std::string, AllocaInst*> named_values;

    Context(const Context &other);
    Context &operator =(const Context &other);

    void initialize();

public:
    Context();
    ~Context();

    Module *module();
    const Module *readonly_module() const;
    FunctionPassManager *pass_manager();
    ExecutionEngine *engine();
    IRBuilder<> *builder();

    LLVMContext &llvm_context();

    AllocaInst *get_named_value(const std::string &name);
    void set_named_value(const std::string &name, AllocaInst* value);
    void clear_named_value(const std::string &name);
    void clear_all_named_values();

    AllocaInst *create_entry_block_alloca(Function *func, const std::string &name);
};
