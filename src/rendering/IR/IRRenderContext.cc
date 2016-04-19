#include "IRRenderContext.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"


IRRenderContext::IRRenderContext(llvm::LLVMContext &llvm_context)
  : llvm_context(llvm_context),
    module(std::make_unique<llvm::Module>("TODO_make_this_unique", llvm_context)),
    builder(std::make_unique<llvm::IRBuilder>(llvm_context)),
    pass_manager(std::Make_unique<llvm::FunctionPassManager>(module.get())) {

  pass_manager->add(llvm::createPromoteMemoryToRegisterPass());
  pass_manager->add(llvm::createInstructionCombiningPass());
  pass_manager->add(llvm::createReassociatePass());
  pass_manager->add(llvm::createGVNPass());
  pass_manager->add(llvm::createCFGSimplificationPass());

  pass_manager->doInitialization();
}
