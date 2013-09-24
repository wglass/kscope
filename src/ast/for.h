#ifndef __FOR_H_INCLUDED__
#define __FOR_H_INCLUDED__

#include <string>

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/context.h"


class ForNode: public ASTNode {
  std::string var_name;
  ASTNode *start, *end, *step, *body;

public:
  ForNode(const std::string &var_name,
          ASTNode *start, ASTNode *end, ASTNode *step,
          ASTNode *body);
  virtual llvm::Value *codegen(Context *context);
};

#endif
