#pragma once

#include "ASTNode.h"


struct IfNode : public ASTNode {
  ASTNode *condition;
  ASTNode *then;
  ASTNode *_else;

  IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else)
    : ASTNode(ASTNodeKind::If),
      condition(cond), then(then), _else(_else) {};
};
