#pragma once

#include "ASTNode.h"

#include "kscope/Render/Renderer.h"

#include "llvm/IR/Value.h"


struct UnaryNode : public ASTNode {
  char opcode;
  ASTNode *operand;

  UnaryNode(char opcode, ASTNode *operand) : opcode(opcode), operand(operand) {};

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
