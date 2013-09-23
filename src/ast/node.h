#ifndef __NODE_H_INCLUDED__
#define __NODE_H_INCLUDED__

#include "llvm/IR/Value.h"

class ASTNode {
public:
  virtual ~ASTNode() {};
  virtual llvm::Value *codegen() = 0;
};

#endif
