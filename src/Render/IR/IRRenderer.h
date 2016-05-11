#pragma once

#include "kscope/Render/Renderer.h"

#include "IRContext.h"
#include "Pipeline/Pipeline.h"

#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include <unordered_map>
#include <vector>
#include <string>


struct PrototypeNode;
class ASTree;

enum class PipelineChoice { Simple, Lazy };
typedef std::vector<std::unique_ptr<llvm::Module>> ModuleSet;
typedef std::unordered_map<std::string, PrototypeNode *> ProtoMap;


class IRRenderer : public Renderer {

public:

  IRRenderer(PipelineChoice pipeline_choice);
  ~IRRenderer();

  llvm::TargetMachine & get_target_machine();
  const llvm::DataLayout get_data_layout();
  IRContext & get_render_context();

  void render_tree(std::shared_ptr<ASTree> tree);

  llvm::orc::TargetAddress get_symbol(const std::string &name);
  PrototypeNode *get_prototype(const std::string &name);

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
  std::unique_ptr<ProtoMap> proto_map;

  std::unique_ptr<llvm::TargetMachine> target_machine;
  const llvm::DataLayout data_layout;
};
