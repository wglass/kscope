#pragma once

#include <string>

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/renderer.h"


class ForNode: public ASTNode {
  std::string var_name;
  ASTNode *start, *end, *step, *body;

public:
  ForNode(const std::string &var_name,
          ASTNode *start, ASTNode *end, ASTNode *step,
          ASTNode *body);
  virtual llvm::Value *codegen(IRRenderer *renderer) override final;
};
