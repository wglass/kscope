#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"

#include <vector>

#include "ast/PrototypeNode.h"
#include "errors/Error.h"

#include "IRRenderer.h"


using ::llvm::AllocaInst;
using ::llvm::Function;
using ::llvm::FunctionType;
using ::llvm::Type;
using ::llvm::Argument;


Function *
IRRenderer::render(PrototypeNode *node) {
    std::vector<Type*> doubles(node->args.size(),
                               Type::getDoubleTy(llvm_context()));
    FunctionType *func_type = FunctionType::get(Type::getDoubleTy(llvm_context()),
                                                doubles,
                                                false);

    Function *func = Function::Create(func_type,
                                      Function::ExternalLinkage,
                                      node->name,
                                      module.get());

    if ( func->getName() != node->name ) {
        func->eraseFromParent();
        func = module->getFunction(node->name);

        if ( !func->empty() ) {
          return Error<Function>::handle("redefinition of function");
        }
        if ( func->arg_size() != node->args.size() ) {
          return Error<Function>::handle("redefintion of function with different number of args");
        }
    }

    Function::arg_iterator iterator = func->arg_begin();
    for ( auto &arg : node->args ) {
      Argument *val = &(*iterator++);
      val->setName(arg);
    }

    return func;
}
