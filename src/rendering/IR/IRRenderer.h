#pragma once

#include "rendering/Renderer.h"

#include "parsing/ASTree.h"

#include "IRRenderSpec.h"
#include "IRContext.h"
#include "ORCPipeline/LazyORCPipeline.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"

#include <map>
#include <string>


class IRRenderer : public Renderer<IRRenderSpec> {

public:
  typedef std::vector<std::unique_ptr<llvm::Module>> ModuleSet;

  IRRenderer();
  ~IRRenderer();

  IRContext & get_render_context();

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
  std::unique_ptr<IRContext> render_context;

  ModuleSet pending_modules;
};
