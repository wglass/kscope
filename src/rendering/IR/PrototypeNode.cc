#include "IRRenderer.h"

#include "ast/PrototypeNode.h"
#include "errors/Error.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"

#include <vector>


llvm::Function *
IRRenderer::render(PrototypeNode *node) {
  auto &context = get_render_context();
  auto &llvm_context = context.get_llvm_context();
  auto &module = context.get_module();

  auto double_type = llvm::Type::getDoubleTy(llvm_context);

  std::vector<llvm::Type*> doubles(node->args.size(), double_type);

  llvm::FunctionType *func_type = llvm::FunctionType::get(double_type,
                                                          doubles,
                                                          false);

  llvm::Function *func = llvm::Function::Create(func_type,
                                                llvm::Function::ExternalLinkage,
                                                node->name,
                                                &module);

  if ( func->getName() != node->name ) {
    func->eraseFromParent();
    func = module.getFunction(node->name);

    if ( !func->empty() ) {
      return Error<llvm::Function>::handle("redefinition of function");
    }
    if ( func->arg_size() != node->args.size() ) {
      return Error<llvm::Function>::handle("redefintion of function with different number of args");
    }
  }

  llvm::Function::arg_iterator iterator = func->arg_begin();
  for ( auto &arg : node->args ) {
    llvm::Argument *val = &(*iterator++);
    val->setName(arg);
  }

  return func;
}
