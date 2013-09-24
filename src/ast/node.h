#ifndef __NODE_H_INCLUDED__
#define __NODE_H_INCLUDED__

#include "llvm/IR/Value.h"

#include "context.h"

class ASTNode {
public:
  virtual ~ASTNode() {};
  virtual llvm::Value *codegen(Context *context) = 0;
};

#endif
