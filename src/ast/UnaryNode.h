#pragma once

#include "ASTNode.h"


class UnaryNode : public ASTNode {
public:
  char opcode;
  ASTNode *operand;

  UnaryNode(char opcode, ASTNode *operand) : opcode(opcode), operand(operand) {};
};
