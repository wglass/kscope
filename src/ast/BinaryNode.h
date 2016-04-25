#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"

#include <string>


struct BinaryNode : public ASTNode {
  char op;
  ASTNode *lhs, *rhs;

  BinaryNode(char op, ASTNode *lhs, ASTNode *rhs)
  : op(op), lhs(lhs), rhs(rhs) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
