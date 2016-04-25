#pragma once

#include <utility>
#include <string>


class IRRenderer;

struct ASTNode {
  virtual ~ASTNode() {};

  virtual void *render(IRRenderer *renderer) = 0;
};
