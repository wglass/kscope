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


template <class RenderResult, typename Function>
class Renderer {
 public:
  virtual void render(std::shared_ptr<ASTree> tree) = 0;

  virtual Function get_function(const std::string &name) = 0;

  virtual RenderResult* render(ASTNode *node) = 0;

  virtual void render(FunctionNode *node) = 0;

  virtual RenderResult* render(BinaryNode *node) = 0;
  virtual RenderResult* render(CallNode *node) = 0;
  virtual RenderResult* render(ForNode *node) = 0;
  virtual RenderResult* render(IfNode *node) = 0;
  virtual RenderResult* render(NumberNode *node) = 0;
  virtual RenderResult* render(PrototypeNode *node) = 0;
  virtual RenderResult* render(UnaryNode *node) = 0;
  virtual RenderResult* render(VarNode *node) = 0;
  virtual RenderResult* render(VariableNode *node) = 0;
};
