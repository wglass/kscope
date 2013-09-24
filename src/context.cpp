#include "llvm/Analysis/Passes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Scalar.h"

#include <string>

#include "context.h"

using namespace llvm;

Context::Context() {
    initialize(new Module("my cool jit", getGlobalContext()));
}

Context::Context(const Context &other) {
    initialize(CloneModule(other.module()));
}

Context
&Context::operator =(const Context &other) {
    if ( this == &other ) {
        return *this;
    }

    delete pass_manager_;
    delete builder_;
    delete engine_;
    delete module_;

    initialize(CloneModule(other.module()));

    return *this;
}

Context::~Context() {
    delete pass_manager_;
    delete builder_;
    delete engine_;
    delete module_;
}

void
Context::initialize(Module *module) {
    module_ = module;

    std::string engine_error;
    engine_ = EngineBuilder(module).setErrorStr(&engine_error).create();

    if ( ! engine_ ) {
        fprintf(stderr,
                "Could not create execution engine: %s\n",
                engine_error.c_str());
        exit(1);
    }

    IRBuilder<> ir_builder = IRBuilder<>(module->getContext());

    builder_ = &ir_builder;

    FunctionPassManager manager = FunctionPassManager(module);

    manager.add(new DataLayout(*engine_->getDataLayout()));
    manager.add(createBasicAliasAnalysisPass());
    manager.add(createPromoteMemoryToRegisterPass());
    manager.add(createInstructionCombiningPass());
    manager.add(createReassociatePass());
    manager.add(createGVNPass());
    manager.add(createCFGSimplificationPass());

    manager.doInitialization();

    pass_manager_ = &manager;
}

const llvm::Module *
Context::module() const { return module_; }

const llvm::ExecutionEngine *
Context::engine() const { return engine_; }

const llvm::FunctionPassManager *
Context::pass_manager() const { return pass_manager_; }

const llvm::IRBuilder<> *
Context::builder() const { return builder_; }

llvm::AllocaInst *
Context::get_named_value (const std::string &name){
    return named_values[name];
}

void
Context::set_named_value(const std::string &name, llvm::AllocaInst *value) {
    named_values[name] = value;
}

void
Context::clear_named_value(const std::string &name) {
    named_values.erase(name);
}

void
Context::clear_all_named_values() {
    named_values.clear();
}
