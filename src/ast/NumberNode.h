#pragma once

#include <string>

#include "ASTNode.h"


class NumberNode : public ASTNode {
public:
  double val;

  NumberNode(double val) : val(val) {};

  template<class RenderSpec> typename RenderSpec::Result* render(Renderer<RenderSpec> *renderer) {
    return renderer->render(this);
  };
};
