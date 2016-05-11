#pragma once

#include "ASTNode.h"

#include "kscope/Render/Renderer.h"

#include "llvm/IR/Value.h"

#include <string>


struct BinaryNode : public ASTNode {
  char op;
  ASTNode *lhs, *rhs;

  BinaryNode(char op, ASTNode *lhs, ASTNode *rhs)
  : op(op), lhs(lhs), rhs(rhs) {};

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
