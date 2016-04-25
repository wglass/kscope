#pragma once

#include "ASTNode.h"
#include "rendering/IR/IRRenderer.h"


struct IfNode : public ASTNode {
  ASTNode *condition;
  ASTNode *then;
  ASTNode *_else;

  IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else)
    : condition(cond), then(then), _else(_else) {};

  void * render(IRRenderer *renderer) {
    return renderer->render_node(this);
  }
};
