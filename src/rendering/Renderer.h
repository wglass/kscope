#pragma once

#include "ast/ASTNode.h"

#include <string>
#include <cstdint>

class ASTree;

struct ASTNode;
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


template <class Subclass, class Spec>
class Renderer {
 public:
  typedef typename Spec::Result Result;
  typedef typename Spec::TopLevelResult TopLevelResult;
  typedef typename Spec::FuncRepr FuncRepr;

  virtual void render_tree(std::shared_ptr<ASTree> tree) = 0;

  virtual FuncRepr get_function(const std::string &name) = 0;

  Result *render(ASTNode *node) {
    void *result = node->render(static_cast<Subclass *>(this));
    return static_cast<Result *>(result);
  }
  Result *render_node(ASTNode *node) {
    return nullptr;
  }

  virtual TopLevelResult *render_node(FunctionNode *node) = 0;
  virtual TopLevelResult *render_node(PrototypeNode *node) = 0;


private:

  virtual Result *render_node(BinaryNode *node) = 0;
  virtual Result *render_node(CallNode *node) = 0;
  virtual Result *render_node(ForNode *node) = 0;
  virtual Result *render_node(IfNode *node) = 0;
  virtual Result *render_node(NumberNode *node) = 0;
  virtual Result *render_node(UnaryNode *node) = 0;
  virtual Result *render_node(VarNode *node) = 0;
  virtual Result *render_node(VariableNode *node) = 0;
};
