#pragma once

#include <string>

#include "ASTNode.h"


class NumberNode : public ASTNode {
public:
  double val;

  NumberNode(double val) : val(val) {};

  template<class Result, typename Function> Result* render(Renderer<Result, Function> *renderer) {
    return renderer->render(this);
  };
};
