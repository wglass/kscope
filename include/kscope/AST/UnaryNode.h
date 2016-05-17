#pragma once

#include "ASTNode.h"


struct UnaryNode : public ASTNode {
  char opcode;
  ASTNode *operand;

  UnaryNode(char opcode, ASTNode *operand)
    : ASTNode(ASTNodeKind::Unary),
      opcode(opcode), operand(operand) {};
};
