#ifndef __FUNCTION_H_INCLUDED__
#define __FUNCTION_H_INCLUDED__

#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include "node.h"
#include "prototype.h"
#include "context.h"

class FunctionNode : public ASTNode {
  PrototypeNode *proto;
  ASTNode *body;

public:
  FunctionNode(PrototypeNode *proto, ASTNode *body);
  llvm::Function *codegen(Context *context);
};

#endif
