#pragma once

#include "ASTNode.h"


class IfNode : public ASTNode {
public:
  std::unique_ptr<ASTNode> condition;
  std::unique_ptr<ASTNode> then;
  std::unique_ptr<ASTNode> _else;

  IfNode(std::unique_ptr<ASTNode> cond,
         std::unique_ptr<ASTNode> then,
         std::unique_ptr<ASTNode> _else)
    : condition(std::move(cond)),
      then(std::move(then)),
      _else(std::move(_else)) {};
};
