# pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/Target/TargetMachine.h"

#include "ast/PrototypeNode.h"


class SessionContext {
public:
  SessionContext(LLVMContext &C)
    : Context(C), TM(EngineBuilder().selectTarget()) {}

  LLVMContext& get_llvm_context();
  TargetMachine& get_target_machine();

  void add_prototype(std::unique_ptr<PrototypeNode> P);
  PrototypeNode* get_prototype(const std::string &name);

private:
  LLVMContext &llvm_ontext;
  std::unique_ptr<TargetMachine> target_machine;

  std::map<std::string, std::unique_ptr<PrototypeNode>> prototypes;
};
