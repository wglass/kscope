#include "llvm/IR/LLVMContext.h"
#include "llvm/Target/TargetMachine.h"

#include "ast/PrototypeNode.h"

#include "SessionContext.h"


LLVMContext &
SessionContext::get_llvm_context const {
  return llvm_context;
}

TargetMachine &
SessionContext::get_target_machine {
  return target_machine;
}

void
SessionContext::add_prototype(std::unique_ptr<PrototypeNode> node) {
  prototypes[node->Name] = std::move(node);
}

PrototypeNode *
SessionContext::get_prototype(const std::string &name) {
  PrototypeMap::iterator iter = prototypes.find(name);
  if (iter != prototypes.end()) {
    return iter->second.get();
  }
  return nullptr;
}
