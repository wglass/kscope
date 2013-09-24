#ifndef __BINARY_H_INCLUDED__
#define __BINARY_H_INCLUDED__

#include <string>

#include "llvm/IR/Value.h"

#include "node.h"
#include "context.h"

class BinaryNode : public ASTNode {
  char op;
  ASTNode *lhs, *rhs;

public:
  BinaryNode(char op, ASTNode *lhs, ASTNode *rhs);
  virtual llvm::Value *codegen(Context *context);
};

#endif
