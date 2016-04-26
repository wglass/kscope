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


class IRRenderer : public Renderer {

public:
  typedef std::vector<std::unique_ptr<llvm::Module>> ModuleSet;

  IRRenderer();
  ~IRRenderer();

  IRContext & get_render_context();

  void render_tree(std::shared_ptr<ASTree> tree);

  llvm::orc::TargetAddress get_function(const std::string &name);
  LazyORCPipeline::ModuleHandle flush_modules();

  llvm::Function *render_function(FunctionNode *node);
  llvm::Function *render_node(FunctionNode *node);
  llvm::Function *render_node(PrototypeNode *node);

  llvm::Value *render_node(BinaryNode *node);
  llvm::Value *render_node(CallNode *node);
  llvm::Value *render_node(ForNode *node);
  llvm::Value *render_node(IfNode *node);
  llvm::Value *render_node(NumberNode *node);
  llvm::Value *render_node(UnaryNode *node);
  llvm::Value *render_node(VarNode *node);
  llvm::Value *render_node(VariableNode *node);

private:
  IRRenderer(const IRRenderer &other) = delete;
  IRRenderer(IRRenderer &&other);

  IRRenderer &operator =(IRRenderer other);

  std::unique_ptr<LazyORCPipeline> pipeline;
  std::unique_ptr<IRContext> render_context;

  ModuleSet pending_modules;
};
