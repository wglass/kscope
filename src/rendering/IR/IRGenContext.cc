#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

#include "ast/PrototypeNode.h"

#include "IRGenContext.h"


IRGenContext::IRGenContext(SessionContext &session)
  : session(session),
    module(new llvm::Module(GenerateUniqueName("jit_module_"),
                            session.getLLVMContext())),
    builder(session.getLLVMContext()) {

    module->setDataLayout(session.getTarget().createDataLayout());
}

llvm::Function* IRGenContext::get_prototype(const std::string &name) {
  if (llvm::Function *existing = module->getFunction(name)) {
    return existing;
  }
  if (PrototypeNode *proto = session.get_prototype(name)) {
    render(proto);
  }

  return nullptr;
}
