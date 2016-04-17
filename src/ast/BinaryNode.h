#pragma once

#include <string>

#include "ASTNode.h"

template<class Result> class Renderer;


class BinaryNode : public ASTNode {
public:
  char op;
  ASTNode *lhs, *rhs;

  BinaryNode(char op, ASTNode *lhs, ASTNode *rhs)
  : op(op), lhs(lhs), rhs(rhs) {};

  template<class Result> Result* render(Renderer<Result> *renderer) {
    renderer->render(this);
  }
};
