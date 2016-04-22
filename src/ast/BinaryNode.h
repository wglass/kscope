#pragma once

#include <string>

#include "ASTNode.h"

template<class Result> class Renderer;


class BinaryNode : public ASTNode {
public:
  char op;
  std::unique_ptr<ASTNode> lhs, rhs;

  BinaryNode(char op, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
    : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {};

  template<class Result> Result* render(Renderer<Result> *renderer) {
    renderer->render(this);
  }
};
