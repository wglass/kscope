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


template <class RenderSpec>
class Renderer {
 public:
  virtual void render(std::shared_ptr<ASTree> tree) = 0;

  virtual typename RenderSpec::FuncRepresentation get_function(const std::string &name) = 0;

  virtual typename RenderSpec::Result* render(ASTNode *node) = 0;

  virtual void render(FunctionNode *node) = 0;

  virtual typename RenderSpec::Result* render(BinaryNode *node) = 0;
  virtual typename RenderSpec::Result* render(CallNode *node) = 0;
  virtual typename RenderSpec::Result* render(ForNode *node) = 0;
  virtual typename RenderSpec::Result* render(IfNode *node) = 0;
  virtual typename RenderSpec::Result* render(NumberNode *node) = 0;
  virtual typename RenderSpec::Result* render(PrototypeNode *node) = 0;
  virtual typename RenderSpec::Result* render(UnaryNode *node) = 0;
  virtual typename RenderSpec::Result* render(VarNode *node) = 0;
  virtual typename RenderSpec::Result* render(VariableNode *node) = 0;
};
