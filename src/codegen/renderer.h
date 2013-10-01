#pragma once

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"

#include <map>
#include <string>

using ::std::map;
using ::std::string;
using ::std::unique_ptr;

using ::llvm::AllocaInst;
using ::llvm::ExecutionEngine;
using ::llvm::Function;
using ::llvm::FunctionPassManager;
using ::llvm::IRBuilder;
using ::llvm::LLVMContext;
using ::llvm::Module;


class IRRenderer {
    map<string, AllocaInst*> named_values;

    IRRenderer(const IRRenderer &other);
    IRRenderer(Module *module);
    IRRenderer(IRRenderer &&other);

    IRRenderer &operator =(IRRenderer other);

public:
    IRRenderer();
    ~IRRenderer();

    unique_ptr<Module> module;
    unique_ptr<ExecutionEngine> engine;
    unique_ptr<IRBuilder<> > builder;
    unique_ptr<FunctionPassManager> pass_manager;

    LLVMContext &llvm_context();

    AllocaInst *get_named_value(const std::string &name);
    void set_named_value(const std::string &name, AllocaInst* value);
    void clear_named_value(const std::string &name);
    void clear_all_named_values();

    AllocaInst *create_entry_block_alloca(Function *func, const std::string &name);
};
