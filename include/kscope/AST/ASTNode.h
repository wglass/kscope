#pragma once

#include "llvm/IR/Value.h"

#include <utility>
#include <string>


class Renderer;

struct ASTNode {
  virtual ~ASTNode() {};

  virtual llvm::Value *render(Renderer *renderer) = 0;
};
