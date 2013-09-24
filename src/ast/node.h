#pragma once

#include "llvm/IR/Value.h"

#include "codegen/context.h"

class ASTNode {
public:
  virtual ~ASTNode() {};
  virtual llvm::Value *codegen(Context *context) = 0;
};
