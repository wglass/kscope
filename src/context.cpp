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

Context::Context()
    : module_(new Module("my cool jit", getGlobalContext())),
      builder_(new IRBuilder<>(module_->getContext())) {
    initialize();
}

Context::Context(const Context &other)
    : module_(CloneModule(other.readonly_module())),
      builder_(new IRBuilder<>(module_->getContext())) {
    initialize();
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

    module_ = CloneModule(other.readonly_module());
    builder_ = new IRBuilder<>(module_->getContext());

    initialize();

    return *this;
}

Context::~Context() {
    delete pass_manager_;
    delete builder_;
    delete engine_;
    delete module_;
}

void
Context::initialize() {
    std::string engine_error;
    engine_ = EngineBuilder(module_).setErrorStr(&engine_error).create();

    if ( ! engine_ ) {
        fprintf(stderr,
                "Could not create execution engine: %s\n",
                engine_error.c_str());
        exit(1);
    }

    pass_manager_ = new FunctionPassManager(module_);

    pass_manager_->add(new DataLayout(*engine_->getDataLayout()));
    pass_manager_->add(createBasicAliasAnalysisPass());
    pass_manager_->add(createPromoteMemoryToRegisterPass());
    pass_manager_->add(createInstructionCombiningPass());
    pass_manager_->add(createReassociatePass());
    pass_manager_->add(createGVNPass());
    pass_manager_->add(createCFGSimplificationPass());

    pass_manager_->doInitialization();
}

llvm::Module *
Context::module() { return module_; }

const llvm::Module *
Context::readonly_module() const { return module_; }

llvm::ExecutionEngine *
Context::engine() { return engine_; }

llvm::FunctionPassManager *
Context::pass_manager() { return pass_manager_; }

llvm::IRBuilder<> *
Context::builder() { return builder_; }

llvm::LLVMContext &
Context::llvm_context() { return module_->getContext(); }

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
