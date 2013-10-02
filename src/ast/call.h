#pragma once

#include <string>
#include <vector>

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/renderer.h"


class CallNode : public ASTNode {
  std::string callee;
  std::vector<ASTNode*> args;

public:
  CallNode(const std::string &callee, std::vector<ASTNode*> &args);
  virtual llvm::Value *codegen(IRRenderer *renderer) override final;
};
