#ifndef __PROTOTYPE_H_INCLUDED__
#define __PROTOTYPE_H_INCLUDED__

#include <string>
#include <vector>

#include "llvm/IR/Module.h"

#include "node.h"

class PrototypeNode : public ASTNode {
  std::string name;
  std::vector<std::string> args;
  bool is_operator;
  unsigned precedence;

public:
  PrototypeNode(const std::string &name,
                const std::vector<std::string> &args,
                bool is_operator = false,
                unsigned precedence = 0);

  bool isUnaryOp() const;
  bool isBinaryOp() const;

  char getOperatorName() const;
  unsigned getBinaryPrecedence() const;

  llvm::Function *codegen();
  void CreateArgumentAllocas(llvm::Function *func);
};

#endif
