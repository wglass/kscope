#ifndef __NUMBER_H_INCLUDED__
#define __NUMBER_H_INCLUDED__

#include "llvm/IR/Value.h"

#include "node.h"
#include "context.h"

class NumberNode : public ASTNode {
  double val;

public:
  NumberNode(double val);
  virtual llvm::Value *codegen(Context *context);
};


#endif
