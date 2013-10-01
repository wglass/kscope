#pragma once

#include "llvm/IR/Value.h"

#include "codegen/renderer.h"

class ASTNode {
public:
  virtual ~ASTNode() {};
  virtual llvm::Value *codegen(IRRenderer *renderer) = 0;
};
