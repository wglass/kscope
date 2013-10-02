#pragma once

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/renderer.h"


class UnaryNode : public ASTNode {
  char opcode;
  ASTNode *operand;

public:
  UnaryNode(char opcode, ASTNode *operand);
  virtual llvm::Value *codegen(IRRenderer *renderer) override final;
};
