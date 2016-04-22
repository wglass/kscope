#pragma once

#include "ASTNode.h"


class UnaryNode : public ASTNode {
public:
  char opcode;
  std::unique_ptr<ASTNode> operand;

  UnaryNode(char opcode, std::unique_ptr<ASTNode> operand)
    : opcode(opcode), operand(std::move(operand)) {};

  template<class Result, typename Function> Result* render(Renderer<Result, Function> *renderer) {
    return renderer->render(this);
  };
};
