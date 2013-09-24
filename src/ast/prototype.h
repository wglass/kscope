#pragma once

#include <string>
#include <vector>

#include "llvm/IR/Module.h"

#include "node.h"
#include "codegen/context.h"


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

  llvm::Function *codegen(Context *context);
  void create_argument_allocas(Context *context, llvm::Function *func);
};
