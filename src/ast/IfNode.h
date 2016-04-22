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

  template<class Result, typename Function> Result* render(Renderer<Result, Function> *renderer) {
    return renderer->render(this);
  };
};
