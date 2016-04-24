#pragma once

#include <string>

#include "ASTNode.h"


class BinaryNode : public ASTNode {
public:
  char op;
  ASTNode *lhs, *rhs;

  BinaryNode(char op, ASTNode *lhs, ASTNode *rhs)
  : op(op), lhs(lhs), rhs(rhs) {};
};
