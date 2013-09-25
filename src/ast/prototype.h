#pragma once

#include <string>
#include <vector>

#include "llvm/IR/Module.h"

#include "node.h"
#include "codegen/context.h"


class PrototypeNode : public ASTNode {
  std::string name;
  std::vector<std::string> args;

public:
  PrototypeNode(const std::string &name,
                const std::vector<std::string> &args);

  llvm::Function *codegen(Context *context);
  void create_argument_allocas(Context *context, llvm::Function *func);
};
