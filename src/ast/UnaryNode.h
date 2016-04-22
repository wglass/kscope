#pragma once

#include "ASTNode.h"


class UnaryNode : public ASTNode {
public:
  char opcode;
  std::unique_ptr<ASTNode> operand;

  UnaryNode(char opcode, std::unique_ptr<ASTNode> operand)
    : opcode(opcode), operand(std::move(operand)) {};
};
