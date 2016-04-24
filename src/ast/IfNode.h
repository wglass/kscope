#pragma once

#include "ASTNode.h"


class IfNode : public ASTNode {
public:
  ASTNode *condition;
  ASTNode *then;
  ASTNode *_else;

  IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else)
    : condition(cond), then(then), _else(_else) {};
};
