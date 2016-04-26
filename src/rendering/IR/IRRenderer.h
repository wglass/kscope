#pragma once

#include "rendering/Renderer.h"

#include "parsing/ASTree.h"

#include "IRContext.h"
#include "rendering/IR/Pipeline/Pipeline.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"

#include <map>
#include <string>


enum class PipelineChoice { Simple, Lazy };
typedef std::vector<std::unique_ptr<llvm::Module>> ModuleSet;


class IRRenderer : public Renderer {

public:

  IRRenderer(PipelineChoice pipeline_choice);
  ~IRRenderer();

  IRContext & get_render_context();

  void render_tree(std::shared_ptr<ASTree> tree);

  llvm::orc::TargetAddress get_function(const std::string &name);
  void flush_modules();

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

  std::unique_ptr<Pipeline> pipeline;
  std::unique_ptr<IRContext> render_context;

  std::unique_ptr<ModuleSet> pending_modules;
};
