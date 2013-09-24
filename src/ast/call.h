#ifndef __CALL_H_INCLUDED__
#define __CALL_H_INCLUDED__

#include <string>
#include <vector>

#include "llvm/IR/Value.h"

#include "node.h"
#include "codegen/context.h"


class CallNode : public ASTNode {
  std::string callee;
  std::vector<ASTNode*> args;

public:
  CallNode(const std::string &callee, std::vector<ASTNode*> &args);
  virtual llvm::Value *codegen(Context *context);
};

#endif
