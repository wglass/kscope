#ifndef __CONTEXT_H_INCLUDED__
#define __CONTEXT_H_INCLUDED__

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/PassManager.h"

#include <map>
#include <string>


class Context {
    llvm::Module *module_;
    llvm::ExecutionEngine *engine_;
    llvm::FunctionPassManager *pass_manager_;
    llvm::IRBuilder<> *builder_;

    std::map<std::string, llvm::AllocaInst*> named_values;

    Context(const Context &other);
    Context &operator =(const Context &other);

    void initialize();

public:
    Context();
    ~Context();

    llvm::Module *module();
    const llvm::Module *readonly_module() const;
    llvm::FunctionPassManager *pass_manager();
    llvm::ExecutionEngine *engine();
    llvm::IRBuilder<> *builder();

    llvm::LLVMContext &llvm_context();

    llvm::AllocaInst *get_named_value(const std::string &name);
    void set_named_value(const std::string &name, llvm::AllocaInst* value);
    void clear_named_value(const std::string &name);
    void clear_all_named_values();
};

#endif
