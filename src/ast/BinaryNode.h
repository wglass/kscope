#pragma once

#include <string>

#include "ASTNode.h"


class BinaryNode : public ASTNode {
public:
  char op;
  std::unique_ptr<ASTNode> lhs, rhs;

  BinaryNode(char op, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
    : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {};

  template<class Result, typename Function> Result* render(Renderer<Result, Function> *renderer) {
    return renderer->render(this);
  };
};
