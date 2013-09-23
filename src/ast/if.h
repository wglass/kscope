#ifndef __IF_H_INCLUDED__
#define __IF_H_INCLUDED__

#include "llvm/IR/Value.h"

#include "node.h"

class IfNode : public ASTNode {
  ASTNode *condition, *then, *_else;

public:
  IfNode(ASTNode *cond, ASTNode *then, ASTNode *_else);
  virtual llvm::Value *codegen();
};

#endif
