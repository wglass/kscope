#pragma once

#include <string>

#include "ASTNode.h"


class ForNode: public ASTNode {
public:
  std::string var_name;
  std::unique_ptr<ASTNode> start, end, step, body;

  ForNode(const std::string &var_name,
          std::unique_ptr<ASTNode> start, std::unique_ptr<ASTNode> end,
          std::unique_ptr<ASTNode> step,
          std::unique_ptr<ASTNode> body)
    : var_name(var_name),
      start(std::move(start)), end(std::move(end)),
      step(std::move(step)), body(std::move(body)) {};

  template<class Result, typename Function> Result* render(Renderer<Result, Function> *renderer) {
    return renderer->render(this);
  };
};
