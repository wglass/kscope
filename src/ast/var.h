#ifndef __VAR_H_INCLUDED__
#define __VAR_H_INCLUDED__

#include <string>
#include <vector>

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/context.h"


class VarNode : public ASTNode {
  std::vector<std::pair<std::string, ASTNode*> > var_names;
  ASTNode *body;

public:
  VarNode(const std::vector<std::pair<std::string, ASTNode*> > &var_names,
          ASTNode *body);
  virtual llvm::Value *codegen(Context *context);
};

#endif
