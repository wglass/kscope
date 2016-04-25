#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"


struct UnaryNode : public ASTNode {
  char opcode;
  ASTNode *operand;

  UnaryNode(char opcode, ASTNode *operand) : opcode(opcode), operand(operand) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
