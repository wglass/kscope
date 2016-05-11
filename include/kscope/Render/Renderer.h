#pragma once

#include "kscope/AST/ASTNode.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"

#include <string>
#include <cstdint>

class ASTree;

struct BinaryNode;
struct CallNode;
struct ForNode;
struct FunctionNode;
struct IfNode;
struct NumberNode;
struct PrototypeNode;
struct UnaryNode;
struct VarNode;
struct VariableNode;


class Renderer {
 public:
  virtual void render_tree(std::shared_ptr<ASTree> tree) = 0;

  virtual llvm::orc::TargetAddress get_symbol(const std::string &name) = 0;

  llvm::Value *render(ASTNode *node) {
    return node->render(this);
  }

  virtual llvm::Function *render_node(FunctionNode *node) = 0;
  virtual llvm::Function *render_node(PrototypeNode *node) = 0;
  virtual llvm::Value *render_node(BinaryNode *node) = 0;
  virtual llvm::Value *render_node(CallNode *node) = 0;
  virtual llvm::Value *render_node(ForNode *node) = 0;
  virtual llvm::Value *render_node(IfNode *node) = 0;
  virtual llvm::Value *render_node(NumberNode *node) = 0;
  virtual llvm::Value *render_node(UnaryNode *node) = 0;
  virtual llvm::Value *render_node(VarNode *node) = 0;
  virtual llvm::Value *render_node(VariableNode *node) = 0;
};
