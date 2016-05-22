#pragma once

#include "IRContext.h"
#include "Pipeline/Pipeline.h"

#include "kscope/AST/ASTVisitor.h"

#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include <unordered_map>
#include <vector>
#include <string>


struct PrototypeNode;

enum class PipelineChoice { Simple, Lazy };
typedef std::vector<std::unique_ptr<llvm::Module>> ModuleSet;
typedef std::unordered_map<std::string, PrototypeNode *> ProtoMap;


class IRRenderer : public ASTVisitor<IRRenderer, llvm::Value> {

public:

  IRRenderer(PipelineChoice pipeline_choice);
  ~IRRenderer();

  llvm::TargetMachine & get_target_machine();
  const llvm::DataLayout get_data_layout();
  IRContext & get_render_context();

  llvm::orc::TargetAddress get_symbol(const std::string &name);

  void add_prototype(PrototypeNode *node);
  PrototypeNode *get_prototype(const std::string &name);

  void flush_modules();

  llvm::Function *render_function(FunctionNode *node);

#define AST_NODE(NODE_NAME) \
  virtual llvm::Value *visit_node(NODE_NAME##Node *node) override;
#include "kscope/AST/ASTNodes.def"

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
