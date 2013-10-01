#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"

#include "ast/prototype.h"
#include "renderer.h"
#include "errors.h"

#include <vector>

using ::llvm::AllocaInst;
using ::llvm::Function;
using ::llvm::FunctionType;
using ::llvm::Type;


void
PrototypeNode::create_argument_allocas(IRRenderer *renderer, Function *func) {
    Function::arg_iterator iterator = func->arg_begin();

    for (unsigned i = 0, size = args.size(); i != size; ++i, ++iterator) {
        AllocaInst *alloca = context->create_entry_block_alloca(func, args[i]);
        context->builder->CreateStore(iterator, alloca);
        context->set_named_value(args[i], alloca);
    }
}

Function *
PrototypeNode::codegen(IRRenderer *renderer) {
    std::vector<Type*> doubles(args.size(),
                               Type::getDoubleTy(renderer->llvm_context()));
    FunctionType *func_type = FunctionType::get(
                                                Type::getDoubleTy(renderer->llvm_context()),
                                                doubles,
                                                false);

    Function *func = Function::Create(func_type,
                                      Function::ExternalLinkage,
                                      name,
                                      renderer->module.get());

    if ( func->getName() != name ) {
        func->eraseFromParent();
        func = renderer->module->getFunction(name);

        if ( !func->empty() ) {
            ErrorF("redefinition of function");
            return 0;
        }
        if ( func->arg_size() != args.size() ) {
            ErrorF("redefintion of function with different number of args");
            return 0;
        }
    }

    unsigned index = 0;
    for (
         Function::arg_iterator iterator = func->arg_begin();
         index != args.size();
         ++iterator, ++index
         ) {
        iterator->setName(args[index]);
    }

    return func;
}
