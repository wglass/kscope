#pragma once

#include <string>
#include <cstdint>

class ASTree;
class ASTNode;
class BinaryNode;
class CallNode;
class ForNode;
class FunctionNode;
class IfNode;
class NumberNode;
class PrototypeNode;
class UnaryNode;
class VarNode;
class VariableNode;


template <class Subclass, class Spec>
class Renderer {
 public:
  virtual void render_tree(std::shared_ptr<ASTree> tree) = 0;

  virtual typename Spec::FuncRepr get_function(const std::string &name) = 0;

  typename Spec::Result *render(ASTNode *node) {
    return static_cast<Subclass *>(this)->render(node);
  }
  virtual typename Spec::TopLevelResult *render_node(FunctionNode *node) = 0;
  virtual typename Spec::TopLevelResult *render_node(PrototypeNode *node) = 0;


private:

  virtual typename Spec::Result *render_node(BinaryNode *node) = 0;
  virtual typename Spec::Result *render_node(CallNode *node) = 0;
  virtual typename Spec::Result *render_node(ForNode *node) = 0;
  virtual typename Spec::Result *render_node(IfNode *node) = 0;
  virtual typename Spec::Result *render_node(NumberNode *node) = 0;
  virtual typename Spec::Result *render_node(UnaryNode *node) = 0;
  virtual typename Spec::Result *render_node(VarNode *node) = 0;
  virtual typename Spec::Result *render_node(VariableNode *node) = 0;
};
