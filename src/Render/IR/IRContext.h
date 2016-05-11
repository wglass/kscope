#pragma once

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"

#include <string>


class IRContext {
public:
  IRContext(const llvm::DataLayout data_layout);
  ~IRContext();

  llvm::LLVMContext & get_llvm_context();
  llvm::Module & get_module();
  llvm::IRBuilder<> & get_builder();
  llvm::legacy::FunctionPassManager & get_pass_manager();

  bool has_module();
  std::unique_ptr<llvm::Module> give_up_module();

  llvm::AllocaInst *get_named_value(const std::string &name);
  void set_named_value(const std::string &name, llvm::AllocaInst* value);
  void clear_named_value(const std::string &name);
  void clear_all_named_values();

  llvm::AllocaInst *create_entry_block_alloca(llvm::Function *func,
                                              const std::string &name);
  void create_argument_allocas(llvm::Function *func,
                               const std::vector<std::string> &args);

private:
  IRContext(const IRContext &other) = delete;
  IRContext(IRContext &&other);

  IRContext &operator =(IRContext other) = delete;

  llvm::LLVMContext &llvm_context;

  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::IRBuilder<>> builder;
  std::unique_ptr<llvm::legacy::FunctionPassManager> pass_manager;

  std::map<std::string, llvm::AllocaInst*> named_values;
};
