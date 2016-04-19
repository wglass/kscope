#pragma once

#include "rendering/Renderer.h"

#include "IRRenderContext.h"
#include "ORCPipelines/ORCPipeline.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

#include <map>
#include <string>


class IRRenderer : public Renderer<llvm::Value> {

public:
  IRRenderer(std::unique_ptr<ORCPipeline> pipeline);
  ~IRRenderer();

  llvm::LLVMContext & get_llvm_context();
  IRRenderContext & get_render_context();

  llvm::AllocaInst *get_named_value(const std::string &name);
  void set_named_value(const std::string &name, AllocaInst* value);
  void clear_named_value(const std::string &name);
  void clear_all_named_values();

  llvm::AllocaInst *create_entry_block_alloca(llvm::Function *func,
                                              const std::string &name);
  void create_argument_allocas(llvm::Function *func,
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

private:
  IRRenderer(const IRRenderer &other);
  IRRenderer(IRRenderer &&other);

  IRRenderer &operator =(IRRenderer other);

  std::unique_ptr<ORCPipeline> pipeline;
  std::unique_ptr<IRRenderContext> render_context;

  llvm::LLVMContext llvm_context;
  std::unique_ptr<llvm::TargetMachine> target_machine;

  std::map<std::string, llvm::AllocaInst*> named_values;
};
