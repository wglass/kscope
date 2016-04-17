#pragma once

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"

#include "rendering/Renderer.h"

#include <map>
#include <string>

using ::llvm::AllocaInst;
using ::llvm::ExecutionEngine;
using ::llvm::Function;
using ::llvm::legacy::FunctionPassManager;
using ::llvm::IRBuilder;
using ::llvm::LLVMContext;
using ::llvm::Module;


class IRRenderer : public Renderer<llvm::Value> {
  std::map<std::string, AllocaInst*> named_values;

  IRRenderer(const IRRenderer &other);
  IRRenderer(std::unique_ptr<Module> module);
  IRRenderer(IRRenderer &&other);

  IRRenderer &operator =(IRRenderer other);

public:
  IRRenderer();
  ~IRRenderer();

  std::unique_ptr<Module> module;
  std::unique_ptr<ExecutionEngine> engine;
  std::unique_ptr<IRBuilder<> > builder;
  std::unique_ptr<FunctionPassManager> pass_manager;

  LLVMContext &llvm_context();

  AllocaInst *get_named_value(const std::string &name);
  void set_named_value(const std::string &name, AllocaInst* value);
  void clear_named_value(const std::string &name);
  void clear_all_named_values();

  AllocaInst *create_entry_block_alloca(Function *func,
                                        const std::string &name);
  void create_argument_allocas(Function *func,
                               const std::vector<std::string> &args);

  llvm::Value *render(ASTNode *node);

  llvm::Value *render(BinaryNode *node);
  llvm::Value *render(CallNode *node);
  llvm::Value *render(ForNode *node);
  llvm::Function *render(FunctionNode *node);
  llvm::Value *render(IfNode *node);
  llvm::Value *render(NumberNode *node);
  llvm::Function *render(PrototypeNode *node);
  llvm::Value *render(UnaryNode *node);
  llvm::Value *render(VarNode *node);
  llvm::Value *render(VariableNode *node);
};
