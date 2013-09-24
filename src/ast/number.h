#pragma once

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/context.h"

class NumberNode : public ASTNode {
  double val;

public:
  NumberNode(double val);
  virtual llvm::Value *codegen(Context *context);
};
