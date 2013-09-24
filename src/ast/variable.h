#ifndef __VARIABLE_H_INCLUDED__
#define __VARIABLE_H_INCLUDED__

#include <string>

#include "llvm/IR/Value.h"

#include "node.h"
#include "context.h"

class VariableNode : public ASTNode {
  std::string name;

public:
  VariableNode(const std::string &name);
  const std::string getName() const;
  virtual llvm::Value *codegen(Context *context);
};


#endif
