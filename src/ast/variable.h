#pragma once

#include <string>

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/renderer.h"


class VariableNode : public ASTNode {
  std::string name;

public:
  VariableNode(const std::string &name);
  const std::string getName() const;
  virtual llvm::Value *codegen(IRRenderer *renderer) override final;
};
