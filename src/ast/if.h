#pragma once

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/renderer.h"


class IfNode : public ASTNode {
  ASTNode *condition, *then, *_else;

public:
  IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else);
  virtual llvm::Value *codegen(IRRenderer *renderer) override final;
};
