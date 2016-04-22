#pragma once

#include "rendering/Renderer.h"

#include "parsing/ASTree.h"

#include "IRRenderContext.h"
#include "ORCPipeline/LazyORCPipeline.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"

#include <map>
#include <string>


class IRRenderer : public Renderer<llvm::Value, llvm::orc::TargetAddress> {

public:
  typedef std::vector<std::unique_ptr<llvm::Module>> ModuleSet;

  IRRenderer();
  ~IRRenderer();

  llvm::LLVMContext & get_llvm_context();
  IRRenderContext & get_render_context();

  llvm::TargetMachine & get_target_machine();

  llvm::orc::TargetAddress get_function(const std::string &name);
  LazyORCPipeline::ModuleHandle flush_modules();

  void render(std::shared_ptr<ASTree> tree);

  llvm::Value *render(ASTNode *node);

  void render(FunctionNode *node);
  llvm::Function *render_function(FunctionNode *node);

  llvm::Function *render(PrototypeNode *node);
  llvm::Value *render(BinaryNode *node);
  llvm::Value *render(CallNode *node);
  llvm::Value *render(ForNode *node);
  llvm::Value *render(IfNode *node);
  llvm::Value *render(NumberNode *node);
  llvm::Value *render(UnaryNode *node);
  llvm::Value *render(VarNode *node);
  llvm::Value *render(VariableNode *node);

private:
  IRRenderer(const IRRenderer &other) = delete;
  IRRenderer(IRRenderer &&other);

  IRRenderer &operator =(IRRenderer other);

  std::unique_ptr<LazyORCPipeline> pipeline;
  std::unique_ptr<IRRenderContext> render_context;

  llvm::LLVMContext &llvm_context;
  std::unique_ptr<llvm::TargetMachine> target_machine;

  ModuleSet pending_modules;

  std::map<std::string, std::unique_ptr<FunctionNode>> unrendered_functions;
};
