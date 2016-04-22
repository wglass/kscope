#pragma once

#include <string>

#include "ASTNode.h"


class ForNode: public ASTNode {
public:
  std::string var_name;
  ASTNode *start, *end, *step, *body;

  ForNode(const std::string &var_name,
          ASTNode *start, ASTNode *end, ASTNode *step,
          ASTNode *body)
    : var_name(var_name), start(start), end(end), step(step), body(body) {};

  template<class RenderSpec> typename RenderSpec::Result* render(Renderer<RenderSpec> *renderer) {
    return renderer->render(this);
  };
};
