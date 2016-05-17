#pragma once

#include "ASTNode.h"


struct BinaryNode : public ASTNode {
  char op;
  ASTNode *lhs, *rhs;

  BinaryNode(char op, ASTNode *lhs, ASTNode *rhs)
    : ASTNode(ASTNodeKind::Binary),
      op(op), lhs(lhs), rhs(rhs) {};
};
