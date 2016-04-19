#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"


class IRRenderContext {
public:
  IRRenderContext(llvm::LLVMContext &llvm_context);
  ~IRRenderContext();

  llvm::LLVMContext & get_llvm_context();
  llvm::Module & get_module();
  llvm::IRBuilder<> & get_builder();
  llvm::FunctionPassManager & get_pass_manager();

  std::unique_ptr<llvm::Module> give_up_module();

private:
  IRRenderContext(const IRRenderContext &other);
  IRRenderContext(IRRenderContext &&other);

  IRRenderContext &operator =(IRRenderContext other);

  llvm::LLVMContext &llvm_context;
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::IRBuilder<>> builder;
  std::unique_ptr<llvm::FunctionPassManager> pass_manager;
};
