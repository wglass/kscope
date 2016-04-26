#pragma once

#include "ASTNode.h"
#include "rendering/Renderer.h"

#include "llvm/IR/Value.h"


struct IfNode : public ASTNode {
  ASTNode *condition;
  ASTNode *then;
  ASTNode *_else;

  IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else)
    : condition(cond), then(then), _else(_else) {};

  llvm::Value * render(Renderer *renderer) {
    return renderer->render_node(this);
  }
};
