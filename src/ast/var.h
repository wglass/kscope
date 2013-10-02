#pragma once

#include <string>
#include <vector>

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/renderer.h"


class VarNode : public ASTNode {
  std::vector<std::pair<std::string, ASTNode*> > var_names;
  ASTNode *body;

public:
  VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names,
          ASTNode *body);
  virtual llvm::Value *codegen(IRRenderer *renderer) override final;
};
