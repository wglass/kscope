#pragma once


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


template <class Result>
class Renderer {
 public:
  Renderer<Result>();
  virtual ~Renderer();

  virtual Result* render(ASTNode *node);

  virtual Result* render(BinaryNode *node) = 0;
  virtual Result* render(CallNode *node) = 0;
  virtual Result* render(ForNode *node) = 0;
  virtual Result* render(FunctionNode *node) = 0;
  virtual Result* render(IfNode *node) = 0;
  virtual Result* render(NumberNode *node) = 0;
  virtual Result* render(PrototypeNode *node) = 0;
  virtual Result* render(UnaryNode *node) = 0;
  virtual Result* render(VarNode *node) = 0;
  virtual Result* render(VariableNode *node) = 0;
};
